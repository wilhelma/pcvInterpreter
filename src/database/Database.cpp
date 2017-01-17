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
#include "QueryResult.h"
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

// Database rows
#include "Access.h"
#include "Call.h"
#include "File.h"
#include "Function.h"
#include "Instruction.h"
#include "Loop.h"
#include "LoopExecution.h"
#include "LoopIteration.h"
#include "Reference.h"
#include "Segment.h"
#include "Thread.h"

#include "Types.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// logging system
#include "easylogging++.h"

#include <iomanip>

// Default destructor.
Database::~Database() = default;

//auto read_task(int limit, int offset) {
//    DBManager db;
//    try {
//        db.open(DBPath);
//    } catch (const SQLException& e) {
//        LOG(FATAL) << e.what();
//        std::abort();
//    }
//}

// Helper function to fill the tables
template<typename T,
         typename = std::enable_if_t<std::is_base_of<DBTable<typename T::index_type, typename T::value_type>, T>::value>>
std::unique_ptr<const T> load_table(const std::string& table_name, const DBManager& db) {
    // Query the number of entries in the database table
    const int table_entries = db.entries(table_name);

    // Create the new table
    T* table = new T();
    table->reserve(table_entries);
    std::copy(SQLStatementIterator<typename T::value_type>(db.query("select * from " + table_name + ";")),
              SQLStatementIterator<typename T::value_type>::end(), inserter<T>(table));

    // Log the number of entries
    LOG(TRACE) << std::setw(24) << std::left
               << "Rows in " + table_name + ": " << table->size();

    // Check if the table size is correct
    assert(static_cast<int>(table->size()) == table_entries);

    return std::unique_ptr<const T>(static_cast<const T*>(table));
}

std::unique_ptr<const Database> load_database(const std::string& DBPath) {
    // try to open the database
    DBManager sql_input_db;
    try {
        sql_input_db.open(DBPath);
    } catch (const SQLException& e) {
        LOG(FATAL) << e.what();
        std::abort();
    }

    // Create the database
    auto db = std::make_unique<const Database>(
            load_table<AccessTable>("Access", sql_input_db),
            load_table<CallTable>("Call", sql_input_db),
            load_table<FileTable>("File", sql_input_db),
            load_table<FunctionTable>("Function", sql_input_db),
            load_table<InstructionTable>("Instruction", sql_input_db),
            load_table<LoopTable>("Loop", sql_input_db),
            load_table<LoopExecutionTable>("LoopExecution", sql_input_db),
            load_table<LoopIterationTable>("LoopIteration", sql_input_db),
            load_table<ReferenceTable>("Reference", sql_input_db),
            load_table<SegmentTable>("Segment", sql_input_db),
            load_table<ThreadTable>("Thread", sql_input_db));

    return db;
}

const access_t& access_of(const instruction_t& ins, const Database& db) {
    const auto& accesses = db.accessTable()->getInsAccessMap().at(ins.id);

    // if this is not 1, how can I know which access I'm referring to?
    if (accesses.size() != 1)
        throw DatabaseException("Instruction refers to more accesses", "access_of");
    return access_with_id(accesses[0], db);
}

const segment_t& segment_of(const instruction_t& ins, const Database& db)
{ return db.segmentTable()->at(ins.segment_id); }

const CAL_ID& caller_id_of(const instruction_t& ins, const Database& db)
{ return segment_of(ins, db).call_id; }

const call_t& caller_of(const instruction_t& ins, const Database& db)
{ return call_of(segment_of(ins, db), db); }

const CAL_ID& call_id_of(const instruction_t& ins, const Database& db) {
    const auto& id_call_of_ins = db.callTable()->instructionToCall().find(ins.id);
    if (id_call_of_ins == db.callTable()->instructionToCall().cend()) {
        // If this happens, the database is corrupted.
        LOG(FATAL) << "Database is corrupted: InstructionToCall has no entry "
                   << ins.id;
        std::abort();
    }

    return id_call_of_ins->second;
}

const call_t& call_of(const segment_t& seg, const Database& db)
{ return db.callTable()->at(seg.call_id); }

const call_t& call_of(const instruction_t& ins, const Database& db) {
    const auto& call_id_of_ins = call_id_of(ins, db); // may throw
    return db.callTable()->at(call_id_of_ins);
}

const reference_t& reference_of(const access_t& acc, const Database& db)
{ return db.referenceTable()->at(acc.reference_id); }

const file_t& file_of(const function_t& fun, const Database& db)
{ return db.fileTable()->at(fun.file_id); }

const function_t& function_of(const call_t& call, const Database& db)
{ return db.functionTable()->at(call.function_id); }

const thread_t& thread_of(const call_t& call, const Database& db)
{ return db.threadTable()->at(call.thread_id); }

const std::vector<ACC_ID>& access_ids_of(const instruction_t& ins, const Database& db) noexcept {
    const auto& acc_ids_of_ins = db.accessTable()->getInsAccessMap().find(ins.id);
    if (acc_ids_of_ins == end(db.accessTable()->getInsAccessMap())) {
        // If this happens, the database is corrupted
        LOG(FATAL) << "Database is corrupted: InsAccessMaps has no entry "
                   << ins.id;
        std::abort();
    }

    return acc_ids_of_ins->second;
}

const access_t& access_with_id(const ACC_ID& acc_id, const Database& db)
{ return db.accessTable()->at(acc_id); }

const call_t& call_with_id(const CAL_ID& cal_id, const Database& db)
{ return db.callTable()->at(cal_id); }

const file_t& file_with_id(const FIL_ID& fil_id, const Database& db)
{ return db.fileTable()->at(fil_id); }

const function_t& function_with_id(const FUN_ID& fun_id, const Database& db)
{ return db.functionTable()->at(fun_id); }

const instruction_t& instruction_with_id(const INS_ID& ins_id, const Database& db)
{ return db.instructionTable()->at(ins_id); }

const reference_t& reference_with_id(const REF_ID& ref_id, const Database& db)
{ return db.referenceTable()->at(ref_id); }

const segment_t& segment_with_id(const SEG_ID& seg_id, const Database& db)
{ return db.segmentTable()->at(seg_id); }

const thread_t& thread_with_id(const TRD_ID& trd_id, const Database& db)
{ return db.threadTable()->at(trd_id); }

const thread_t& thread_forked_by(const instruction_t& ins, const Database& db) {
    if (ins.instruction_type != InstructionType::FORK)
        throw DatabaseException("Instruction " + std::to_string(ins.id) + " is not a fork", "thread_forked_by");

    for (const auto& trd : *db.threadTable())
        if (ins.id == trd.create_instruction_id)
            return trd;

    // If the function doesn't return, throw
    throw SQLException("No thread was forked by instruction " + std::to_string(ins.id), "thread_forked_by");  
}

const thread_t& thread_joined_by(const instruction_t& ins, const Database& db) {
    if (ins.instruction_type != InstructionType::JOIN)
        throw DatabaseException("Instruction " + std::to_string(ins.id) + " is not a join", "thread_joined_by");

    for (const auto& trd : *db.threadTable())
        if (ins.id == trd.join_instruction_id)
            return trd;

    // If the function doesn't return, throw
    throw SQLException("No thread was joined by instruction " + std::to_string(ins.id), "thread_joined_by");  
}
