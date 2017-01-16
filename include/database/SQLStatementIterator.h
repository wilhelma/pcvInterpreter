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
    constexpr explicit SQLStatementIterator(std::shared_ptr<QueryResult> query)
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
    static constexpr const SQLStatementIterator end() noexcept
    { return SQLStatementIterator{}; }
    
    /// @brief Equality operator.
    /// @details Checks if pointers to statement are the same.
    friend constexpr const bool operator==(const SQLStatementIterator& lhs, const SQLStatementIterator& rhs) noexcept
    { return lhs.Query_ == rhs.Query_; }

private:
    /// @brief Pointer to QueryResult, which manages the query result.
    /// @attention This can't be a `unique_ptr` because the iterator needs
    /// to be copied in `std::copy`.
    std::shared_ptr<QueryResult> Query_;

    /// Default constructor (private).
    constexpr explicit SQLStatementIterator() noexcept :  Query_(nullptr) {};
};

/// @brief Unequality operator.
/// @details Negates `operator==`.
template <typename T>
inline constexpr const bool operator!=(const SQLStatementIterator<T>& lhs, const SQLStatementIterator<T>& rhs) noexcept
{ return !(lhs == rhs); }

/// Deference operator (`const access_t` specialization).
template<>
inline access_t SQLStatementIterator<access_t>::operator*() const {
    auto&&           id(Query_->get<ACC_ID>(0));
    auto&&  instruction(Query_->get<INS_ID>(1));
    auto&&     position(Query_->get<POS>(2));
    auto&&    reference(Query_->get<REF_ID>(3));
    auto&&  access_type(Query_->get<AccessType>(4));
    auto&& memory_state(Query_->get<AccessState>(5));

    return access_t(id, instruction, position, reference, access_type, memory_state);
}

/// Deference operator (`const call_t` specialization).
template<>
inline call_t SQLStatementIterator<call_t>::operator*() const {
    auto&&             id(Query_->get<CAL_ID>(0));
    auto&&      thread_id(Query_->get<TRD_ID>(1));
    auto&&    function_id(Query_->get<FUN_ID>(2));
    auto&& instruction_id(Query_->get<INS_ID>(3));
    auto&&     start_time(Query_->get<TIME>(4));
    auto&&       end_time(Query_->get<TIME>(5));

    return call_t(id, thread_id, function_id, instruction_id, start_time, end_time);
}

/// Deference operator (`const file_t` specialization).
template<>
inline file_t SQLStatementIterator<file_t>::operator*() const {
    auto&&        id(Query_->get<FIL_ID>(0));
    auto&& file_path(Query_->get<FIL_PT>(1));

    return file_t(id, file_path);
}

/// Deference operator (`const function_t` specialization).
template<>
inline function_t SQLStatementIterator<function_t>::operator*() const {
    auto&&          id(Query_->get<FUN_ID>(0));
    auto&&        name(Query_->get<FUN_NM>(1));
    auto&&   signature(Query_->get<FUN_SG>(2));
    auto&&     file_id(Query_->get<FIL_ID>(3));
    auto&& line_number(Query_->get<LIN_NO>(4));
    auto&&        type(Query_->get<FunctionType>(6));

    return function_t(id, name, signature, type, file_id, line_number);
}

/// Deference operator (`const instruction_t` specialization).
template<>
inline instruction_t SQLStatementIterator<instruction_t>::operator*() const {
    auto&&      id(Query_->get<INS_ID>(0));
    auto&& segment(Query_->get<SEG_ID>(1));
    auto&&    type(Query_->get<InstructionType>(2));
    auto&&    line(Query_->get<LIN_NO>(3));

    return instruction_t(id, segment, type, line);
}

/// @brief Deference operator (`const loopExecution_t` specialization).
/// @todo Define types for `parent` and `duration`
template<>
inline loopExecution_t SQLStatementIterator<loopExecution_t>::operator*() const {
    auto&&   id(Query_->get<LOE_ID>(0));
    auto&& loop(Query_->get<LOE_ID>(1));
    // TODO Define types for these variables!
    auto&& parent{Query_->get<int>(2)};
    auto&& duration{Query_->get<int>(3)};

    return loopExecution_t(id, loop, parent, duration);
}

/// Deference operator (`const loopIteration_t` specialization).
template<>
inline loopIteration_t SQLStatementIterator<loopIteration_t>::operator*() const {
    auto&&            id(Query_->get<LOI_ID>(0));
    auto&& loopExecution(Query_->get<LOE_ID>(1));
    auto&& loopIteration(Query_->get<LOI_ID>(2));

    return loopIteration_t(id, loopExecution, loopIteration);
}

/// Deference operator (`const loop_t` specialization).
template<>
inline loop_t SQLStatementIterator<loop_t>::operator*() const {
    auto&&   id(Query_->get<LOP_ID>(0));
    auto&& line(Query_->get<LIN_NO>(1));

    return loop_t(id, line);
}

/// Deference operator (`const reference_t` specialization).
template<>
inline reference_t SQLStatementIterator<reference_t>::operator*() const {
    auto&&         id(Query_->get<REF_ID>(0));
    auto&&       size(Query_->get<REF_SIZE>(1));
    auto&&       type(Query_->get<ReferenceType>(2));
    auto&& allocinstr(Query_->get<INS_ID>(4));
    auto&&       name(Query_->get<REF_NAME>(3));

    return reference_t(id, size, type, name, allocinstr);
}

/// Deference operator (`const segment_t` specialization).
template<>
inline segment_t SQLStatementIterator<segment_t>::operator*() const {
    auto&&           id(Query_->get<SEG_ID>(0));
    auto&&         call(Query_->get<CAL_ID>(1));
    auto&&         type(Query_->get<SegmentType>(2));
    auto&& loop_pointer(Query_->get<LOI_ID>(3));

    return segment_t(id, call, type, loop_pointer);
}

/// Deference operator (`const thread_t` specialization).
template<>
inline thread_t SQLStatementIterator<thread_t>::operator*() const {
    auto&&               id(Query_->get<TRD_ID>(0));
    auto&&       start_time(Query_->get<TIME_STRING>(1));
    auto&&         end_time(Query_->get<TIME_STRING>(2));
    auto&&      start_cycle(Query_->get<TIME>(3));
    auto&&       num_cycles(Query_->get<TIME>(4));
    auto&&  create_instr_id(Query_->get<INS_ID>(5));
    auto&&    join_instr_id(Query_->get<INS_ID>(6));
    auto&& parent_thread_id(Query_->get<TRD_ID>(7));
    auto&&       process_id(Query_->get<PID>(8));
    auto&&          call_id(Query_->get<CAL_ID>(9));

    return thread_t(id, start_time, end_time, start_cycle, num_cycles, create_instr_id, join_instr_id, parent_thread_id, process_id, call_id);
}

/// @brief Deference operator.
/// @details `size_t` specialization to query for the number of entries in tables.
template <>
inline size_t SQLStatementIterator<size_t>::operator*() const {
    return Query_->get<size_t>(0);
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
