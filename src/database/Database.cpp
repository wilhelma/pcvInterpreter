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
#include <future>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// logging system
#include "easylogging++.h"

#include <iomanip>

// Default destructor.
Database::~Database() = default;

// Helper function to evaluate the number of available threads on the machine
const size_t available_threads() noexcept {
    const auto available_cores = std::thread::hardware_concurrency();

    // This will be 0 if the number of threads cannot be detected on the
    // machine. In this case, I don't use any parallelization.
    return available_cores + (!available_cores ? 1 : 0);
}

// Helper function to distribute the entries to read across tasks
const std::vector<size_t> partition_entries(const std::string& table_name, const std::string& DBPath) {
    const auto db = open_database_connection(DBPath);

    // Query the number of entries in the database table
    const auto table_entries = entries(table_name, db); // may throw
    const auto entries_block = std::min(table_entries, available_threads());

    // If there are no entry in the database table
    if (entries_block == 0)
        return std::vector<size_t>(1, 0);

    std::vector<size_t> entries_partition(entries_block, table_entries / entries_block);
    // Distribute the remainder of the tasks
    for (size_t i = 0; i < table_entries % entries_block; ++ i)
        ++ entries_partition[i];

    return entries_partition;
}

// Helper function to collect the results from the tasks
template <typename T>
const T* collect_results(std::vector<std::future<std::vector<typename T::value_type>>>& task_results, const size_t table_entries) {
    T* table = new T(); // may throw
    table->reserve(table_entries);
    for (auto& tr : task_results) {
        auto result = tr.get();
        std::copy(std::make_move_iterator(std::begin(result)),
                  std::make_move_iterator(std::end(result)),
                  inserter<T>(table));
    }

    return table;
}

// Helper function to fill the tables
template<typename T,
         typename = std::enable_if_t<std::is_base_of<DBTable<typename T::index_type, typename T::value_type>, T>::value>>
std::unique_ptr<const T> load_table(const std::string& table_name, const std::string& DBPath) {

    // Evaluate the the read-in work distribution
    const auto entries_partition = partition_entries(table_name, DBPath);

    auto read_task = [](const std::string table_name, const std::string DBPath, const size_t offset, const size_t table_entries) {
        const auto connection = open_database_connection(DBPath);
        // Create the new table
        std::vector<typename T::value_type> table;
        table.reserve(table_entries);
        std::copy(SQLStatementIterator<typename T::value_type>(query_table(table_name, connection, offset, table_entries)),
                SQLStatementIterator<typename T::value_type>::end(), std::inserter(table, std::end(table)));
        return table;
    };

    // Vector of the task results
    std::vector<std::future<std::vector<typename T::value_type>>> task_results;
    task_results.reserve(entries_partition.size());
    
    // Distribute the tasks
    size_t table_entries = 0;
    for (const auto& ep : entries_partition) {
        task_results.push_back(std::async(std::launch::async, read_task, table_name, DBPath, table_entries, ep));
        table_entries += ep;
    }

    // Collect the results
    const auto table = collect_results<T>(task_results, table_entries);

    // Log the number of entries
    LOG(TRACE) << std::setw(24) << std::left
               << "Rows in " + table_name + ": " << table->size();

    // Check if the table size is correct
    assert(table->size() == table_entries);

    return std::unique_ptr<const T>(table);
}

std::unique_ptr<const Database> load_database(const std::string& DBPath) {
    // Open a new database connection and set WAL mode _only once_
    const auto db = open_database_connection(DBPath);
    db.query("pragma journal_mode = WAL");

    // Create the database
    return std::make_unique<const Database>(
            load_table<AccessTable>("Access", DBPath),
            load_table<CallTable>("Call", DBPath),
            load_table<FileTable>("File", DBPath),
            load_table<FunctionTable>("Function", DBPath),
            load_table<InstructionTable>("Instruction", DBPath),
            load_table<LoopTable>("Loop", DBPath),
            load_table<LoopExecutionTable>("LoopExecution", DBPath),
            load_table<LoopIterationTable>("LoopIteration", DBPath),
            load_table<ReferenceTable>("Reference", DBPath),
            load_table<SegmentTable>("Segment", DBPath),
            load_table<ThreadTable>("Thread", DBPath));
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
