/**
 *
 *    @file  Database.cpp
 *   @brief  
 *
 *    @date  10/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "Database.h"

#include "DatabaseException.h"
#include "DBManager.h"
#include "SQLException.h"
#include "SQLStatementIterator.h"

// Tables
#include "AccessTable.h"
#include "CallTable.h"
#include "FileTable.h"
#include "FunctionTable.h"
#include "InstructionTable.h"
#include "LoopTable.h"
#include "LoopExecutionTable.h"
#include "LoopIterationTable.h"
#include "ReferenceTable.h"
#include "SegmentTable.h"
#include "ThreadTable.h"
#include "DBTable.h"

#include <memory>
#include <string>

// logging system
#include "easylogging++.h"

// Helper function to fill the tables
template<typename IdT, typename T>
inline void fill(const std::string& query_string, const DBManager& db, DBTable<IdT, T>& table) {
	std::copy(SQLStatementIterator<T>(db.query(query_string)),
			  SQLStatementIterator<T>::end(), inserter(table));
}

std::unique_ptr<const Database> load_database(const std::string& DBPath) {
	// try to open the database
	DBManager sql_input_db;
	try {
		sql_input_db.open(DBPath);
	} catch (const SQLException& e) {
		LOG(FATAL) <<e.what();
		std::abort();
	}

    auto db = std::make_unique<Database>();

    // Read from all the tables in the database
    fill("SELECT * from Access;",        sql_input_db, db->accessTable());
    fill("SELECT * from Call;",          sql_input_db, db->callTable());
    fill("SELECT * from File;",          sql_input_db, db->fileTable());
    fill("SELECT * from Function;",      sql_input_db, db->functionTable());
    fill("SELECT * from Instruction;",   sql_input_db, db->instructionTable());
    fill("SELECT * from Loop;",          sql_input_db, db->loopTable());
    fill("SELECT * from LoopExecution;", sql_input_db, db->loopExecutionTable());
    fill("SELECT * from LoopIteration;", sql_input_db, db->loopIterationTable());
    fill("SELECT * from Reference;",     sql_input_db, db->referenceTable());
    fill("SELECT * from Segment;",       sql_input_db, db->segmentTable());
    fill("SELECT * from Thread;",        sql_input_db, db->threadTable());

    LOG(TRACE) << "Rows in Access:        " << db->accessTable().size();
    LOG(TRACE) << "Rows in Call:          " << db->callTable().size();
    LOG(TRACE) << "Rows in File:          " << db->fileTable().size();
    LOG(TRACE) << "Rows in Function:      " << db->functionTable().size();
    LOG(TRACE) << "Rows in Instruction:   " << db->instructionTable().size();
    LOG(TRACE) << "Rows in Loop:          " << db->loopTable().size();
    LOG(TRACE) << "Rows in LoopExecution: " << db->loopExecutionTable().size();
    LOG(TRACE) << "Rows in LoopIteration: " << db->loopIterationTable().size();
    LOG(TRACE) << "Rows in Reference:     " << db->referenceTable().size();
    LOG(TRACE) << "Rows in Segment:       " << db->segmentTable().size();
    LOG(TRACE) << "Rows in Thread:        " << db->threadTable().size();

    return std::unique_ptr<const Database>(std::move(db));
}

const segment_t& segment_of(const instruction_t& ins, const Database& db) noexcept {
    const auto& seg_of_ins = db.segmentTable().find(ins.segment_id);
    if (seg_of_ins == db.segmentTable().cend()) {
        // If this happens, the database is corrupted.
        LOG(FATAL) << "Database is corrupted: SegmentTable has no entry "
                   << ins.segment_id;
        std::abort();
    }

    return seg_of_ins->second;
}

const CAL_ID& caller_id_of(const instruction_t& ins, const Database& db) noexcept
{ return segment_of(ins, db).call_id; }

const call_t& caller_of(const instruction_t& ins, const Database& db) noexcept {
    return call_of(segment_of(ins, db), db);
}

const CAL_ID& call_id_of(const instruction_t& ins, const Database& db) {
    const auto& id_call_of_ins = db.callTable().instructionToCall().find(ins.id);
    if (id_call_of_ins == db.callTable().instructionToCall().cend()) {
        // If this happens, the database is corrupted.
        LOG(FATAL) << "Database is corrupted: InstructionToCall has no entry "
                   << ins.id;
        std::abort();
    }

    return id_call_of_ins->second;
}

const call_t& call_of(const segment_t& seg, const Database& db) noexcept {
    const auto& call_of_seg = db.callTable().find(seg.call_id);
    if (call_of_seg == db.callTable().cend()) {
        // If this happens, the database is corrupted.
        LOG(FATAL) << "Database is corrupted: CallTable has no entry "
                   << seg.call_id;
        std::abort();
    }

    return call_of_seg->second;
}

const call_t& call_of(const instruction_t& ins, const Database& db) noexcept {
    const auto& call_id_of_ins = call_id_of(ins, db); // may throw

    const auto& call_of_ins = db.callTable().find(call_id_of_ins);
    if (call_of_ins == db.callTable().cend()) {
        // If this happens, the database is corrupted.
        LOG(FATAL) << "Database is corrupted: CallTable has no entry "
                   << call_id_of_ins;
        std::abort();
    }
    
    return call_of_ins->second;
}

const reference_t& reference_of(const access_t& acc, const Database& db) noexcept {
    const auto& ref_of_acc = db.referenceTable().find(acc.reference_id);
    if (ref_of_acc == db.referenceTable().cend()) {
        // If this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: ReferenceTable has no entry "
                   << acc.reference_id;
        std::abort();
    }

    return ref_of_acc->second;
}

const file_t& file_of(const function_t& fun, const Database& db) noexcept {
    const auto& file_of_fun = db.fileTable().find(fun.file_id);
    if (file_of_fun == db.fileTable().cend()) {
        // If this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: FileTable has no entry "
                   << fun.file_id;
        std::abort();
    }

    return file_of_fun->second;
}

const function_t& function_of(const call_t& call, const Database& db) noexcept {
    const auto& fun_of_call = db.functionTable().find(call.function_id);
    if (fun_of_call == db.functionTable().cend()) {
        // if this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: FunctionTable has no entry "
                   << call.function_id;
        std::abort();
    }

    return fun_of_call->second;
}

const thread_t& thread_of(const call_t& call, const Database& db) noexcept {
    const auto& fun_of_call = db.threadTable().find(call.thread_id);
    if (fun_of_call == db.threadTable().cend()) {
        // if this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: ThreadTable has no entry "
                   << call.thread_id;
        std::abort();
    }

    return fun_of_call->second;
}

const std::vector<ACC_ID>& access_ids_of(const instruction_t& ins, const Database& db) noexcept {
    const auto& acc_ids_of_ins = db.accessTable().getInsAccessMap().find(ins.id);
    if (acc_ids_of_ins == db.accessTable().getInsAccessMap().cend()) {
        // If this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: InsAccessMaps has no entry "
                   << ins.id;
        std::abort();
    }

    return acc_ids_of_ins->second;
}

const access_t& access_with_id(const ACC_ID& acc_id, const Database& db) {
    const auto& access = db.accessTable().find(acc_id);
    if (access == db.accessTable().cend())
        throw DatabaseException("AccessTable has no entry with id " + std::to_string(acc_id),
                                "access_with_id");

    return access->second;
}

const thread_t& thread_with_id(const TRD_ID& trd_id, const Database& db) {
    const auto& thread = db.threadTable().find(trd_id);
    if (thread == db.threadTable().cend())
        throw DatabaseException("ThreadTable has no entry with id " + std::to_string(trd_id),
                                "thread_with_id");

    return thread->second;
}

const call_t& call_with_id(const CAL_ID& cal_id, const Database& db) {
    const auto& call = db.callTable().find(cal_id);
    if (call == db.callTable().cend())
        throw DatabaseException("CallTable has no entry with id " + std::to_string(cal_id),
                                "call_with_id");

    return call->second;
}
