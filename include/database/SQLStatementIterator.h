///   @file  SQLStatementIterator.h
///
///   @date  07/19/16
/// @author  Paolo Di Giglio (github.com/pdigiglio),
///         <p.digiglio91@gmail.com>

#ifndef  SQL_STATEMENT_ITERATOR_H_
#define  SQL_STATEMENT_ITERATOR_H_

// Database structures -----------
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
// -------------------------------

#include "QueryResult.h"
#include "SQLException.h"
#include "Types.h"

#include <memory>
#include <iterator>

/// @ingroup input
/// @brief Iterates over the rows of the QueryResult resulting from a database query.
template<typename T>
class SQLStatementIterator : public std::iterator<std::input_iterator_tag, QueryResult> {
public:

    /// @brief Constructor.
    /// @param query The query result manager statement to read values in.
    SQLStatementIterator(std::shared_ptr<QueryResult> query)
        : Query_(std::move(query))
    { ++(*this); }

    /// @brief Deference operator.
    /// @details This is declared but not defined. Only certain
    /// specializations of it _can and should_ be used.
    T operator*() const;

    /// @brief Arrow operator.
    /// @return `i->m` is the same as `(*i).m`.
    const T operator->() const
    { return (*this).operator*(); }

    /// @brief Pre-increment operator.
    /// @details Reads the next line of the query statement. When the query
    /// is over, the iterator is set to `end()` and returned.
    /// @throw SQLException 
    SQLStatementIterator& operator++();

    /// Post-increment operator (calls the pre-increment one).
    /// @throw SQLException 
    SQLStatementIterator operator++(int);

    /// Constructs and returns and empty iterator.
    static const SQLStatementIterator& end()
    { static SQLStatementIterator sql_statement_end; return sql_statement_end; }
    
    /// @brief Equality operator.
    /// @details Checks if pointers to statement are the same.
    friend const bool operator==(const SQLStatementIterator& lhs, const SQLStatementIterator& rhs)
    { return lhs.Query_ == rhs.Query_; }

private:
    /// @brief Pointer to QueryResult, which manages the query result.
    /// @attention This can't be a `unique_ptr` because the iterator needs
    /// to be copied in `std::copy`.
    std::shared_ptr<QueryResult> Query_;

    /// Default constructor (private).
    explicit SQLStatementIterator(): Query_(nullptr) {};
};

/// @brief Unequality operator.
/// @details Negates `operator==`.
template <typename T>
inline const bool operator!=(const SQLStatementIterator<T>& lhs, const SQLStatementIterator<T>& rhs)
{ return !(lhs == rhs); }

/// Deference operator (`const access_t` specialization).
template<>
const access_t SQLStatementIterator<const access_t>::operator*() const {
    ACC_ID                id(Query_->get<ACC_ID>(0));
    INS_ID       instruction(Query_->get<INS_ID>(1));
    POS             position(Query_->get<POS>(2));
    REF_ID         reference(Query_->get<REF_ID>(3));
    AccessType   access_type(Query_->get<AccessType>(4));
    AccessState memory_state(Query_->get<AccessState>(5));

    return access_t(id, instruction, position, reference, access_type, memory_state);
}

/// Deference operator (`const call_t` specialization).
template<>
const call_t SQLStatementIterator<const call_t>::operator*() const {
    CAL_ID             id(Query_->get<CAL_ID>(0));
    TRD_ID      thread_id(Query_->get<TRD_ID>(1));
    FUN_ID    function_id(Query_->get<FUN_ID>(2));
    INS_ID instruction_id(Query_->get<INS_ID>(3));
    TIME       start_time(Query_->get<TIME>(4));
    TIME         end_time(Query_->get<TIME>(5));

    return call_t(id, thread_id, function_id, instruction_id, start_time, end_time);
}

/// Deference operator (`const file_t` specialization).
template<>
const file_t SQLStatementIterator<const file_t>::operator*() const {
    FIL_ID        id(Query_->get<FIL_ID>(0));
    FIL_PT file_path(Query_->get<FIL_PT>(1));

    return file_t(id, file_path);
}

/// Deference operator (`const function_t` specialization).
template<>
const function_t SQLStatementIterator<const function_t>::operator*() const {
    FUN_ID          id(Query_->get<FUN_ID>(0));
    FUN_SG   signature(Query_->get<FUN_SG>(2));
    FIL_ID     file_id(Query_->get<FIL_ID>(3));
    LIN_NO line_number(Query_->get<LIN_NO>(4));
    FunctionType  type(Query_->get<FunctionType>(6));

    return function_t(id, signature, type, file_id, line_number);
}

/// Deference operator (`const instruction_t` specialization).
template<>
const instruction_t SQLStatementIterator<const instruction_t>::operator*() const {
    INS_ID            id(Query_->get<INS_ID>(0));
    SEG_ID       segment(Query_->get<SEG_ID>(1));
    InstructionType type(Query_->get<InstructionType>(2));
    LIN_NO          line(Query_->get<LIN_NO>(3));

    return instruction_t(id, segment, type, line);
}

/// @brief Deference operator (`const loopExecution_t` specialization).
/// @todo Define types for `parent` and `duration`
template<>
const loopExecution_t SQLStatementIterator<const loopExecution_t>::operator*() const {
    LOE_ID   id(Query_->get<LOE_ID>(0));
    LOE_ID loop(Query_->get<LOE_ID>(1));
    // TODO Define types for these variables!
    int parent   = Query_->get<int>(2);
    int duration = Query_->get<int>(3);

    return loopExecution_t(id, loop, parent, duration);
}

/// Deference operator (`const loopIteration_t` specialization).
template<>
const loopIteration_t SQLStatementIterator<const loopIteration_t>::operator*() const {
    LOI_ID            id(Query_->get<LOI_ID>(0));
    LOE_ID loopExecution(Query_->get<LOE_ID>(1));
    LOI_ID loopIteration(Query_->get<LOI_ID>(2));

    return loopIteration_t(id, loopExecution, loopIteration);
}

/// Deference operator (`const loop_t` specialization).
template<>
const loop_t SQLStatementIterator<const loop_t>::operator*() const {
    LOP_ID   id(Query_->get<LOP_ID>(0));
    LIN_NO line(Query_->get<LIN_NO>(1));

    return loop_t(id, line);
}

/// Deference operator (`const reference_t` specialization).
template<>
const reference_t SQLStatementIterator<const reference_t>::operator*() const {
    REF_ID          id(Query_->get<REF_ID>(0));
    REF_SIZE      size(Query_->get<REF_SIZE>(1));
    ReferenceType type(Query_->get<ReferenceType>(2));
    INS_ID  allocinstr(Query_->get<INS_ID>(4));
    REF_NAME      name(Query_->get<REF_NAME>(3));

    return reference_t(id, size, type, name, allocinstr);
}

/// Deference operator (`const segment_t` specialization).
template<>
const segment_t SQLStatementIterator<const segment_t>::operator*() const {
    SEG_ID           id(Query_->get<SEG_ID>(0));
    CAL_ID         call(Query_->get<CAL_ID>(1));
    SegmentType    type(Query_->get<SegmentType>(2));
    LOI_ID loop_pointer(Query_->get<LOI_ID>(3));

    return segment_t(id, call, type, loop_pointer);
}

/// Deference operator (`const thread_t` specialization).
template<>
const thread_t SQLStatementIterator<const thread_t>::operator*() const {
    TRD_ID               id(Query_->get<TRD_ID>(0));
    TIME_STRING  start_time(Query_->get<TIME_STRING>(1));
    TIME_STRING    end_time(Query_->get<TIME_STRING>(2));
    TIME        start_cycle(Query_->get<NUM_CYCLES>(3));
    TIME         num_cycles(Query_->get<NUM_CYCLES>(3));
    INS_ID  create_instr_id(Query_->get<INS_ID>(4));
    INS_ID    join_instr_id(Query_->get<INS_ID>(5));
    TRD_ID parent_thread_id(Query_->get<TRD_ID>(6));
    PID          process_id(Query_->get<PID>(7));
    CAL_ID          call_id(Query_->get<CAL_ID>(9));

    return thread_t(id, start_time, end_time, start_cycle, num_cycles, create_instr_id, join_instr_id, parent_thread_id, process_id, call_id);
}

template<typename T>
SQLStatementIterator<T>& SQLStatementIterator<T>::operator++() {
    switch(Query_->step()) {
        case SQLITE_ROW:
            break;
        case SQLITE_DONE:
            *this = end();
            break;
        default:
            throw SQLException("Iterating DB failed", "SQLStatementIterator::operator++");
            break;
    }

    return *this;
}

template <typename T>
SQLStatementIterator<T> SQLStatementIterator<T>::operator++(int) {
    SQLStatementIterator previous(*this);
    // increment query statement
    ++(*this);
    return previous;
}
#endif
