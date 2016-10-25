/**
 *
 *    @file  Database.h
 *   @brief  
 *
 *    @date  10/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef DATABASE_H_
#define DATABASE_H_

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

#include <memory>

/// @ingroup database
/// @brief Mimic the structure of a database.
class Database
{
public:
    /// _Default_ constructor.
    explicit Database() = default;

    /// Return the database AccessTable_.
    const AccessTable& accessTable() const noexcept
    { return AccessTable_; }

    /// Access the database AccessTable_.
    AccessTable& accessTable() noexcept
    { return AccessTable_; }

    /// Return the database CallTable_.
    const CallTable& callTable() const noexcept
    { return CallTable_; }

    /// Access the database CallTable_.
    CallTable& callTable() noexcept
    { return CallTable_; }

    /// Return the database FileTable_.
    const FileTable& fileTable() const noexcept
    { return FileTable_; }
    
    /// Access the database FileTable_.
    FileTable& fileTable() noexcept
    { return FileTable_; }

    /// Return the database FunctionTable_.
    const FunctionTable& functionTable() const noexcept
    { return FunctionTable_; }

    /// Access the database FunctionTable_.
    FunctionTable& functionTable() noexcept
    { return FunctionTable_; }

    /// Return the database InstructionTable_.
    const InstructionTable& instructionTable() const noexcept
    { return InstructionTable_; }

    /// Access the database InstructionTable_.
    InstructionTable& instructionTable() noexcept
    { return InstructionTable_; }

    /// Return the database LoopTable_.
    const LoopTable& loopTable() const noexcept
    { return LoopTable_; }

    /// Access the database LoopTable_.
    LoopTable& loopTable() noexcept
    { return LoopTable_; }

    /// Return the database LoopExecutionTable_.
    const LoopExecutionTable& loopExecutionTable() const noexcept
    { return LoopExecutionTable_; }

    /// Access the database LoopExecutionTable_.
    LoopExecutionTable& loopExecutionTable() noexcept
    { return LoopExecutionTable_; }

    /// Return the database LoopIterationTable_.
    const LoopIterationTable& loopIterationTable() const noexcept
    { return LoopIterationTable_; }

    /// Access the database LoopIterationTable_.
    LoopIterationTable& loopIterationTable() noexcept
    { return LoopIterationTable_; }

    /// Return the database ReferenceTable_.
    const ReferenceTable& referenceTable() const noexcept
    { return ReferenceTable_; }

    /// Access the database ReferenceTable_.
    ReferenceTable& referenceTable() noexcept
    { return ReferenceTable_; }

    /// Return the database SegmentTable_.
    const SegmentTable& segmentTable() const noexcept
    { return SegmentTable_; }

    /// Access the database SegmentTable_.
    SegmentTable& segmentTable() noexcept
    { return SegmentTable_; }

    /// Return the database ThreadTable_.
    const ThreadTable& threadTable() const noexcept
    { return ThreadTable_; }

    /// Access the database ThreadTable_.
    ThreadTable& threadTable() noexcept
    { return ThreadTable_; }

private:
    /// Map from the Access IDs to the Access-table rows imported from the DB.
    AccessTable        AccessTable_;
    /// Map from the Call IDs to the Call-table rows imported from the DB.
    CallTable          CallTable_;
    /// Map from the File IDs to the File-table rows imported from the DB.
    FileTable          FileTable_;
    /// Map from the Function IDs to the Function-table rows imported from the DB.
    FunctionTable      FunctionTable_;
    /// Map from the Instruction IDs to the Instruction-table rows imported from the DB.
    InstructionTable   InstructionTable_;
    /// Map from the Loop IDs to the Loop-table rows imported from the DB.
    LoopTable          LoopTable_;
    /// Map from the Loop Execution IDs to the Loop-Execution-table rows imported from the DB.
    LoopExecutionTable LoopExecutionTable_;
    /// Map from the Loop Iteration IDs to the Loop-Iteration-table rows imported from the DB.
    LoopIterationTable LoopIterationTable_;
    /// Map from the Reference IDs to the Reference-table rows imported from the DB.
    ReferenceTable     ReferenceTable_;
    /// Map from the Segment IDs to the Segment-table rows imported from the DB.
    SegmentTable       SegmentTable_;
    /// Map from the Thread IDs to the Thread-table rows imported from the DB.
    ThreadTable        ThreadTable_;
};

/// Fill the Database tables.
std::unique_ptr<const Database> load_database(const std::string& DBPath);

/// @brief Segment containing the instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
const segment_t& segment_of(const instruction_t& ins, const Database& db) noexcept;

/// @brief ID of the parent call of an instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
const CAL_ID& caller_id_of(const instruction_t ins, const Database& db) noexcept;

/// @brief Parent call of an instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
const call_t& caller_of(const instruction_t ins, const Database& db) noexcept;

/// @brief ID of the call_t that was called on an instruction.
/// @param ins The call instruction.
/// @param db  The database to look in.
/// @exception DatabaseException Throws if the entry is not found!
const CAL_ID& call_id_of(const instruction_t& ins, const Database& db);

/// @brief The call_t that was called on an instruction.
/// @param ins The call instruction.
/// @param db  The database to look in.
const call_t& call_of(const instruction_t& ins, const Database& db) noexcept;

/// @brief The call that spanned a segment.
/// @param seg The segment.
/// @param db  The database to look in.
const call_t& call_of(const segment_t& seg, const Database& db) noexcept;

/// @brief The reference of the memory access.
/// @param acc The memory access.
/// @param db  The database to look in.
const reference_t& reference_of(const access_t& acc, const Database& db) noexcept;

/// @brief The file a function belongs to.
/// @param fun The function.
/// @param db  The database to look in.
const file_t& file_of(const function_t& fun, const Database& db) noexcept;

/// @brief The function issued by the call.
/// @todo verify this description.
/// @param call The call.
/// @param db   The database to look in.
const function_t& function_of(const call_t& call, const Database& db) noexcept;

/// @brief The thread in which the call runs.
/// @param call The call.
/// @param db   The database to look in.
const thread_t& thread_of(const call_t& call, const Database &db) noexcept;

/// @brief IDs of the memory accesses on an instruction.
/// @param ins The accessing instruction.
/// @param db  The database to look in.
const std::vector<ACC_ID>& access_ids_of(const instruction_t& ins, const Database& db) noexcept;

/// @brief Return the access with a specified access ID.
/// @param acc_id The access ID.
/// @param db     The database to look in.
/// @throw DatabaseException If the entry is not found.
const access_t& access_with_id(const ACC_ID& acc_id, const Database& db);

/// @brief Return the thread with a specified thread ID.
/// @param trd_id The thread ID.
/// @param db     The database to look in.
/// @throw DatabaseException If the entry is not found.
const thread_t& thread_with_id(const TRD_ID& trd_id, const Database& db);

/// @brief Return the call with a specified call ID.
/// @param cal_id The call ID.
/// @param db     The database to look in.
/// @throw DatabaseException If the entry is not found.
const call_t& call_with_id(const CAL_ID& cal_id, const Database& db);

#endif
