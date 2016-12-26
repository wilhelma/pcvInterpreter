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

// Database tables
#include "fwd/AccessTable.h"
#include "fwd/CallTable.h"
#include "fwd/FileTable.h"
#include "fwd/FunctionTable.h"
#include "fwd/InstructionTable.h"
#include "fwd/LoopTable.h"
#include "fwd/LoopExecutionTable.h"
#include "fwd/LoopIterationTable.h"
#include "fwd/ReferenceTable.h"
#include "fwd/SegmentTable.h"
#include "fwd/ThreadTable.h"

// Database rows
#include "fwd/Access.h"
#include "fwd/Call.h"
#include "fwd/File.h"
#include "fwd/Function.h"
#include "fwd/Instruction.h"
#include "fwd/Loop.h"
#include "fwd/LoopExecution.h"
#include "fwd/LoopIteration.h"
#include "fwd/Reference.h"
#include "fwd/Segment.h"
#include "fwd/Thread.h"

#include "Types.h"

#include <memory>
#include <vector>

/// @ingroup database
/// @brief Mimic the structure of a database, being a read-only collection of tables.
class Database
{
public:
    /// @brief Constructor.
    /// @details The database tables are supposed to be loaded by an external
    /// function as this class doesn't provide any method to do it. The table
    /// ownership must then be transferred to Database through this constructor.
    /// @param access_table         The AccessTable pointer.
    /// @param call_table           The CallTable pointer.
    /// @param file_table           The FileTable pointer.
    /// @param function_table       The FunctionTable pointer.
    /// @param instruction_table    The InstructionTable pointer.
    /// @param loop_table           The LoopTable pointer.
    /// @param loop_execution_table The LoopExecutionTable pointer.
    /// @param loop_iteration_table The LoopIterationTable pointer.
    /// @param reference_table      The ReferenceTable pointer.
    /// @param segment_table        The SegmentTable pointer.
    /// @param thread_table         The ThreadTable pointer.
    explicit Database(std::unique_ptr<const AccessTable>&& access_table,
                      std::unique_ptr<const CallTable>&& call_table,
                      std::unique_ptr<const FileTable>&& file_table,
                      std::unique_ptr<const FunctionTable>&& function_table,
                      std::unique_ptr<const InstructionTable>&& instruction_table,
                      std::unique_ptr<const LoopTable>&& loop_table,
                      std::unique_ptr<const LoopExecutionTable>&& loop_execution_table,
                      std::unique_ptr<const LoopIterationTable>&& loop_iteration_table,
                      std::unique_ptr<const ReferenceTable>&& reference_table,
                      std::unique_ptr<const SegmentTable>&& segment_table,
                      std::unique_ptr<const ThreadTable>&& thread_table) :
        AccessTable_(std::move(access_table)),
        CallTable_(std::move(call_table)),
        FileTable_(std::move(file_table)),
        FunctionTable_(std::move(function_table)),
        InstructionTable_(std::move(instruction_table)),
        LoopTable_(std::move(loop_table)),
        LoopExecutionTable_(std::move(loop_execution_table)),
        LoopIterationTable_(std::move(loop_iteration_table)),
        ReferenceTable_(std::move(reference_table)),
        SegmentTable_(std::move(segment_table)),
        ThreadTable_(std::move(thread_table))
    {}

    /// _Default_ destructor.
    ~Database();

    /// Return the database AccessTable_.
    const std::unique_ptr<const AccessTable>& accessTable() const noexcept
    { return AccessTable_; }

    /// Return the database CallTable_.
    const std::unique_ptr<const CallTable>& callTable() const noexcept
    { return CallTable_; }

    /// Return the database FileTable_.
    const std::unique_ptr<const FileTable>& fileTable() const noexcept
    { return FileTable_; }
    
    /// Return the database FunctionTable_.
    const std::unique_ptr<const FunctionTable>& functionTable() const noexcept
    { return FunctionTable_; }

    /// Return the database InstructionTable_.
    const std::unique_ptr<const InstructionTable>& instructionTable() const noexcept
    { return InstructionTable_; }

    /// Return the database LoopTable_.
    const std::unique_ptr<const LoopTable>& loopTable() const noexcept
    { return LoopTable_; }

    /// Return the database LoopExecutionTable_.
    const std::unique_ptr<const LoopExecutionTable>& loopExecutionTable() const noexcept
    { return LoopExecutionTable_; }

    /// Return the database LoopIterationTable_.
    const std::unique_ptr<const LoopIterationTable>& loopIterationTable() const noexcept
    { return LoopIterationTable_; }

    /// Return the database ReferenceTable_.
    const std::unique_ptr<const ReferenceTable>& referenceTable() const noexcept
    { return ReferenceTable_; }

    /// Return the database SegmentTable_.
    const std::unique_ptr<const SegmentTable>& segmentTable() const noexcept
    { return SegmentTable_; }

    /// Return the database ThreadTable_.
    const std::unique_ptr<const ThreadTable>& threadTable() const noexcept
    { return ThreadTable_; }

private:
    /// Map from the Access IDs to the Access-table rows imported from the DB.
    const std::unique_ptr<const AccessTable>        AccessTable_;
    /// Map from the Call IDs to the Call-table rows imported from the DB.
    const std::unique_ptr<const CallTable>          CallTable_;
    /// Map from the File IDs to the File-table rows imported from the DB.
    const std::unique_ptr<const FileTable>          FileTable_;
    /// Map from the Function IDs to the Function-table rows imported from the DB.
    const std::unique_ptr<const FunctionTable>      FunctionTable_;
    /// Map from the Instruction IDs to the Instruction-table rows imported from the DB.
    const std::unique_ptr<const InstructionTable>   InstructionTable_;
    /// Map from the Loop IDs to the Loop-table rows imported from the DB.
    const std::unique_ptr<const LoopTable>          LoopTable_;
    /// Map from the Loop Execution IDs to the Loop-Execution-table rows imported from the DB.
    const std::unique_ptr<const LoopExecutionTable> LoopExecutionTable_;
    /// Map from the Loop Iteration IDs to the Loop-Iteration-table rows imported from the DB.
    const std::unique_ptr<const LoopIterationTable> LoopIterationTable_;
    /// Map from the Reference IDs to the Reference-table rows imported from the DB.
    const std::unique_ptr<const ReferenceTable>     ReferenceTable_;
    /// Map from the Segment IDs to the Segment-table rows imported from the DB.
    const std::unique_ptr<const SegmentTable>       SegmentTable_;
    /// Map from the Thread IDs to the Thread-table rows imported from the DB.
    const std::unique_ptr<const ThreadTable>        ThreadTable_;
};

/// @brief Fill the Database tables.
/// @param DBPath The database path.
std::unique_ptr<const Database> load_database(const std::string& DBPath);

/// @brief Segment containing the instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding segment is not found.
const segment_t& segment_of(const instruction_t& ins, const Database& db);

/// @brief ID of the parent call of an instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding call is not found.
const CAL_ID& caller_id_of(const instruction_t ins, const Database& db);

/// @brief Parent call of an instruction.
/// @param ins The called instruction.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding call is not found.
const call_t& caller_of(const instruction_t ins, const Database& db);

/// @brief ID of the call_t that was called on an instruction.
/// @param ins The call instruction.
/// @param db  The database to look in.
/// @exception DatabaseException Throws if the entry is not found!
const CAL_ID& call_id_of(const instruction_t& ins, const Database& db);

/// @brief The call_t that was called on an instruction.
/// @param ins The call instruction.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding call is not found.
const call_t& call_of(const instruction_t& ins, const Database& db);

/// @brief The call that spanned a segment.
/// @param seg The segment.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding call is not found.
const call_t& call_of(const segment_t& seg, const Database& db);

/// @brief The reference of the memory access.
/// @param acc The memory access.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding reference is not found.
const reference_t& reference_of(const access_t& acc, const Database& db);

/// @brief The file a function belongs to.
/// @param fun The function.
/// @param db  The database to look in.
/// @throws std::out_of_range If the corresponding file is not found.
const file_t& file_of(const function_t& fun, const Database& db);

/// @brief The function issued by the call.
/// @todo verify this description.
/// @param call The call.
/// @param db   The database to look in.
/// @throws std::out_of_range If the corresponding function is not found.
const function_t& function_of(const call_t& call, const Database& db);

/// @brief The thread in which the call runs.
/// @param call The call.
/// @param db   The database to look in.
/// @throws std::out_of_range If the corresponding thread is not found.
const thread_t& thread_of(const call_t& call, const Database &db);

/// @brief IDs of the memory accesses on an instruction.
/// @param ins The accessing instruction.
/// @param db  The database to look in.
const std::vector<ACC_ID>& access_ids_of(const instruction_t& ins, const Database& db) noexcept;

/// @brief Return the access with a specified access ID.
/// @param acc_id The access ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const access_t& access_with_id(const ACC_ID& acc_id, const Database& db);

/// @brief Return the call with a specified call ID.
/// @param cal_id The call ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const call_t& call_with_id(const CAL_ID& cal_id, const Database& db);

/// @brief Return the file with a specified file ID.
/// @param fil_id The file ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const file_t& file_with_id(const FIL_ID& fil_id, const Database& db);

/// @brief Return the function with a specified function ID.
/// @param fun_id The function ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const function_t& function_with_id(const FUN_ID& fun_id, const Database& db);

/// @brief Return the instruction with a specified instruction ID.
/// @param ins_id The instruction ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const instruction_t& instruction_with_id(const INS_ID& ins_id, const Database& db);

/// @brief Return the reference with a specified reference ID.
/// @param ref_id The reference ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const reference_t& reference_with_id(const REF_ID& ref_id, const Database& db);

/// @brief Return the segment with a specified segment ID.
/// @param seg_id The segment ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const segment_t& segment_with_id(const SEG_ID& seg_id, const Database& db);

/// @brief Return the thread with a specified thread ID.
/// @param trd_id The thread ID.
/// @param db     The database to look in.
/// @throw std::out_of_range If the entry is not found.
const thread_t& thread_with_id(const TRD_ID& trd_id, const Database& db);

#endif
