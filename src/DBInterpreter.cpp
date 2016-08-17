/**
 *  @file  DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */
#include "DBInterpreter.h"

#include "fwd/ShadowLock.h"
#include "fwd/ShadowThread.h"
#include "fwd/ShadowVar.h"

#include <iostream>
#include <string>
#include <functional>

#include "SQLStatementIterator.h"

#include "DBTable.h"
#include "DBManager.h"

#include "LockMgr.h"
#include "ThreadMgr.h"

#include "AccessEvent.h"
#include "AccessInfo.h"
#include "AcquireEvent.h"
#include "AcquireInfo.h"
#include "CallEvent.h"
#include "CallInfo.h"
#include "JoinEvent.h"
#include "JoinInfo.h"
#include "NewThreadEvent.h"
#include "NewThreadInfo.h"
#include "ReleaseEvent.h"
#include "ReleaseInfo.h"
#include "ReturnEvent.h"
#include "ReturnInfo.h"
#include "ThreadEndEvent.h"
#include "ThreadEndInfo.h"

#include "EventService.h"
#include "AccessInfo.h"

// logging system
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Helper function to initialize the log.
void initialize_logger(const std::string& logFileName) {
	namespace logging = boost::log;
	namespace expr    = boost::log::expressions;

	logging::add_file_log(
			logging::keywords::file_name = logFileName,
			logging::keywords::format = (
				expr::stream
				<< expr::attr< unsigned int >("LineID")
				<< ": <" << logging::trivial::severity
				<< "> " << expr::smessage
				)	
			);

	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::trace);
	logging::add_common_attributes();
}

DBInterpreter::DBInterpreter(const std::string& logFile, EventService *service, LockMgr *lockMgr, ThreadMgr *threadMgr) :
	EventService_(service), lockMgr_(lockMgr), threadMgr_(threadMgr)
{
	initialize_logger(logFile);
//	CallStack_.push(call_t::MAIN);
};


template<typename IdT, typename T>
inline void fill(const std::string& query_string, const DBManager& db, DBTable<IdT, T>& table) {
	std::copy(SQLStatementIterator<T>(db.query(query_string)),
			  SQLStatementIterator<T>::end(), inserter(table));
}

void DBInterpreter::importDB(const std::string& DBPath) { 
	// try to open the database
	DBManager db;
	try {
		db.open(DBPath);
	} catch (const SQLException& e) {
		BOOST_LOG_TRIVIAL(fatal) << e.what();
		std::abort();
	}

    // Read from all the tables in the database
    fill("SELECT * from Access;",        db, AccessTable_);
    fill("SELECT * from Call;",          db, CallTable_);
    fill("SELECT * from File;",          db, FileTable_);
    fill("SELECT * from Function;",      db, FunctionTable_);
    fill("SELECT * from Instruction;",   db, InstructionTable_);
    fill("SELECT * from Loop;",          db, LoopTable_);
    fill("SELECT * from LoopExecution;", db, LoopExecutionTable_);
    fill("SELECT * from LoopIteration;", db, LoopIterationTable_);
    fill("SELECT * from Reference;",     db, ReferenceTable_);
    fill("SELECT * from Segment;",       db, SegmentTable_);
    fill("SELECT * from Thread;",        db, ThreadTable_);

    BOOST_LOG_TRIVIAL(trace) << "Rows in Access:        " << AccessTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Call:          " << CallTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in File:          " << FileTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Function:      " << FunctionTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Instruction:   " << InstructionTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Loop:          " << LoopTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopExecution: " << LoopExecutionTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in LoopIteration: " << LoopIterationTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Reference:     " << ReferenceTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Segment:       " << SegmentTable_.size();
    BOOST_LOG_TRIVIAL(trace) << "Rows in Thread:        " << ThreadTable_.size();
}

ErrorCode DBInterpreter::process(const std::string& DBPath) {
	// fill internal tables with the database entries
	try {
		importDB(DBPath);
	} catch (const SQLException& e) {
		// this should only happen if the database can't be closed
		BOOST_LOG_TRIVIAL(fatal) << e.what();
		std::abort();
	}

    // process database entries
    for (const auto& instruction : InstructionTable_)
        processInstruction(instruction.second);

    return ErrorCode::OK;
}

const CAL_ID DBInterpreter::getCallerID(const instruction_t& ins) const {
	auto search = SegmentTable_.find(ins.segment_id);
	if (search == SegmentTable_.cend()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id
			<< " not found in SegmentTable_";
		return static_cast<CAL_ID>(static_cast<unsigned>(ErrorCode::NO_ENTRY));
	}
	return search->second.call_id;
}

const CAL_ID DBInterpreter::getCallID(const instruction_t& ins) const {
	// look for the call id of the instruction
	CAL_ID call_id(0);
	bool found = false;
	for (const auto& it : CallTable_) {
		// if there's a call whose instruction id is the same as
		// ins.id, get its id
		if (it.second.id == ins.id) {
			call_id = it.second.id;
			found = true;
			break;
		}
	}

	// if the iterator reached the end, no entry has been found in callT_
	if (!found) {
		BOOST_LOG_TRIVIAL(error) << "Call table has no element whose instruction id is: "
			<< ins.id;
		return static_cast<CAL_ID>(static_cast<unsigned>(ErrorCode::NO_ENTRY));
	}

	std::cerr << "[getCallID]     call_id = " << call_id << std::endl;
	return call_id;
}

ErrorCode DBInterpreter::processReturn(const instruction_t& ins) {
	// Search for the instruction segment in the SegmentTable_.
	auto search = SegmentTable_.find(ins.segment_id);
	if (search == SegmentTable_.cend()) {
		BOOST_LOG_TRIVIAL(error) << "Segment " << ins.segment_id << "not found";
		return ErrorCode::NO_ENTRY;
	}

	if (!CallStack_.isEmpty()) {
		CAL_ID parent_call_id = search->second.call_id;
		while (parent_call_id != CallStack_.top()) {
			CallStack_.pop();

			// in this case, the call didn't happen in the
			// parent ID scope, so the "parent" call has returned
			auto parent_call = CallTable_.find(parent_call_id);
			ReturnInfo info(parent_call_id, parent_call->second.end_time);
			auto thread = threadMgr_->getThread(parent_call->second.thread_id);
			ReturnEvent event(thread, &info);
			getEventService()->publish(&event);
		}
	}

	CAL_ID call_id(getCallID(ins));
	std::cerr << "[processReturn] call_id = " << call_id << std::endl;
	CallStack_.push(call_id);
	return ErrorCode::OK;
}

ErrorCode DBInterpreter::processInstruction(const instruction_t& ins) {
    processAccess_t accessFunc = nullptr;

	SegmentTable::const_iterator search_segment = SegmentTable_.find(ins.segment_id);
	CallTable::const_iterator search_call = CallTable_.cend(); // dumb value
	if (search_segment != SegmentTable_.end()) {
		switch (ins.instruction_type) {
			case InstructionType::CALL:
				// XXX this doesn't change `accessFunc`
				processSegment(ins.segment_id, search_segment->second, ins);
				processReturn(ins);
				break;
			case InstructionType::ACCESS:
				search_call = CallTable_.find(search_segment->second.call_id);
				if (search_call != CallTable_.cend())
					accessFunc = &DBInterpreter::processMemAccess;
				break;
            case InstructionType::ACQUIRE:
				search_call = CallTable_.find(search_segment->second.call_id);
				if (search_call != CallTable_.cend())
					accessFunc = &DBInterpreter::processAcqAccess;
				break;
      case InstructionType::RELEASE:
				search_call = CallTable_.find(search_segment->second.call_id);
				if (search_call != CallTable_.cend())
					accessFunc = &DBInterpreter::processRelAccess;
				break;
			case InstructionType::FORK:
				for (const auto& it : ThreadTable_) {
					const thread_t& thread = it.second;
					if (ins.id == thread.create_instruction_id) {
						search_call = CallTable_.find(search_segment->second.call_id);
						if (search_call != CallTable_.cend())
							processFork(ins, search_segment->second, search_call->second, thread);
					}
				}
				break;
			case InstructionType::JOIN:
				for (const auto& it : ThreadTable_) {
					const thread_t& thread = it.second;
					if (ins.id == thread.join_instruction_id) {
						search_call = CallTable_.find(search_segment->second.call_id);
						if (search_call != CallTable_.cend())
							processJoin(ins, search_segment->second, search_call->second, thread);
					}
				}
				break;
			default:
				return ErrorCode::NO_ENTRY;
		}
	}

	if (accessFunc != nullptr) {
		// loop over all memory accesses of the instruction
    const AccessTable::insAccessMap_t& insAccessMap = AccessTable_.getInsAccessMap();
    const auto& avIt = insAccessMap.find(ins.id);
    if (avIt != insAccessMap.end()) {
      for (const auto& it : avIt->second) {

        auto searchAccess = AccessTable_.find(it);
        if (searchAccess != AccessTable_.end()) {

          processAccessGeneric(searchAccess->first,
              searchAccess->second,
              ins,
              search_segment->second,
              search_call->second,
              accessFunc);
        } else {
          BOOST_LOG_TRIVIAL(error) << "Access not found: " << it;
          return ErrorCode::NO_ENTRY;
        }
      }
    }
	}

	return ErrorCode::OK;
}

ErrorCode DBInterpreter::processSegment(SEG_ID segmentId,
                                  const segment_t& seg,
                                  const instruction_t& ins) {

	auto search = CallTable_.find(seg.call_id);
	if (search == CallTable_.end()) {
		BOOST_LOG_TRIVIAL(error) << "Call not found: " << seg.call_id;
		return ErrorCode::NO_ENTRY;
	}
	
	return processCall(seg.call_id, search->second, seg, ins);
}

size_t getHash(unsigned funId, unsigned lineNo) {
    size_t h1 = std::hash<unsigned>()(funId);
    size_t h2 = std::hash<unsigned>()(lineNo);
    return h1 ^ (h2 << 1);
}

ErrorCode DBInterpreter::processCall(CAL_ID callId,
                               const call_t& call,
                               const segment_t& seg,
                               const instruction_t& ins) {

	// fetch called function
	auto search = FunctionTable_.find(call.function_id);

	// check that the function exists
	if (search == FunctionTable_.end()) {
		BOOST_LOG_TRIVIAL(error) << "Function not found: " << call.function_id;
		return ErrorCode::NO_ENTRY;
	}

	switch(search->second.type) {
		case FunctionType::FUNCTION:
		case FunctionType::METHOD:
			{
				// serch for the file where the call has happened from
				auto searchFile = FileTable_.find(search->second.file_id);
				if (searchFile == FileTable_.end()) {
					BOOST_LOG_TRIVIAL(error) << "File not found: " << search->second.file_id;
					return ErrorCode::NO_ENTRY;
				}

				CallInfo info( (CALLSITE)getHash(call.function_id, ins.line_number),
						(TIME) (call.end_time - call.start_time),
						(FUN_SG)search->second.signature,
						(SEG_ID) call.id, // XXX BUG segment or call?
						search->second.type,
						(FIL_PT)searchFile->second.file_name,
						(FIL_PT)searchFile->second.file_path);

				auto thread = threadMgr_->getThread(call.thread_id);
				CallEvent event(thread, &info);
				getEventService()->publish(&event);
				break;
			}
		default:
			break;
	}

	return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAccessGeneric(ACC_ID accessId,
                                        const access_t& access,
                                        const instruction_t& instruction,
                                        const segment_t& segment,
                                        const call_t& call,
                                        processAccess_t func) {

    REF_ID refId = access.reference_id;
    auto search = ReferenceTable_.find(refId);
    if ( search == ReferenceTable_.end() ) {
        BOOST_LOG_TRIVIAL(error) << "Reference not found: " << access.reference_id;
        return ErrorCode::NO_ENTRY;
	}

    return (this->* func)(accessId, access, instruction, segment,
                       call, search->second);
}

ErrorCode DBInterpreter::processMemAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    ShadowVar *var = 0;
    auto searchVar = _shadowVarMap.find(reference.id);
    if ( searchVar != _shadowVarMap.end() ) {
        var = searchVar->second;
    } else {
        var = new ShadowVar(reference.memory_type,
                            reference.id,
                            reference.size,
                            reference.name);
        _shadowVarMap[reference.id] = var;
    }

    auto thread = threadMgr_->getThread(call.thread_id);
    AccessInfo info( access.access_type,
                     var,
                     instruction.id);
    AccessEvent event( thread, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processAcqAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    auto thread = threadMgr_->getThread(call.thread_id);
    ShadowLock *lock = lockMgr_->getLock(reference.id);
    AcquireInfo info(lock);
    AcquireEvent event( thread, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processRelAccess(ACC_ID accessId,
                                    const access_t& access,
                                    const instruction_t& instruction,
                                    const segment_t& segment,
                                    const call_t& call,
                                    const reference_t& reference) {

    auto thread = threadMgr_->getThread(call.thread_id);
    ShadowLock *lock = lockMgr_->getLock(reference.id);
    ReleaseInfo info(lock);
    ReleaseEvent event( thread, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processFork(const instruction_t& instruction,
                               const segment_t& segment,
                               const call_t& call,
                               const thread_t& thread) {
    auto pT = threadMgr_->getThread(thread.parent_thread_id);
    auto cT = threadMgr_->getThread(thread.id);
    NewThreadInfo info(cT, pT, thread.num_cycles, thread.start_time);
    NewThreadEvent event( pT, &info );
    getEventService()->publish( &event );

    ThreadEndInfo  end_info(call.end_time, thread.id);
    ThreadEndEvent end_event(cT, &end_info);
    getEventService()->publish(&end_event);

    return ErrorCode::OK;
}

ErrorCode DBInterpreter::processJoin(const instruction_t& instruction,
                               const segment_t& segment,
                               const call_t& call,
                               const thread_t& thread) {

    auto pT = threadMgr_->getThread(thread.parent_thread_id);
    auto cT = threadMgr_->getThread(thread.id);
    JoinInfo info(cT, pT);
    JoinEvent event( pT, &info );
    getEventService()->publish( &event );

    return ErrorCode::OK;
}
