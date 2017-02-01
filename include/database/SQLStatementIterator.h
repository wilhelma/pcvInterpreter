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
    constexpr explicit SQLStatementIterator(std::shared_ptr<const QueryResult>&& query)
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
    std::shared_ptr<const QueryResult> Query_;

    /// Default constructor (private).
    constexpr explicit SQLStatementIterator() noexcept : Query_(nullptr) {};
};

/// @brief Unequality operator.
template <typename T>
inline constexpr const bool operator!=(const SQLStatementIterator<T>& lhs, const SQLStatementIterator<T>& rhs) noexcept
{ return !(lhs == rhs); }

/// Deference operator (`const access_t` specialization).
template<>
inline access_t SQLStatementIterator<access_t>::operator*() const {
    return access_t(
            Query_->get<ACC_ID>(0),
            Query_->get<INS_ID>(1),
            Query_->get<POS>(2),
            Query_->get<REF_ID>(3),
            Query_->get<AccessType>(4),
            Query_->get<AccessState>(5));
}

/// Deference operator (`const call_t` specialization).
template<>
inline call_t SQLStatementIterator<call_t>::operator*() const {
    return call_t(
            Query_->get<CAL_ID>(0),
            Query_->get<TRD_ID>(1),
            Query_->get<FUN_ID>(2),
            Query_->get<INS_ID>(3),
            Query_->get<TIME>(4),
            Query_->get<TIME>(5));
}

/// Deference operator (`const file_t` specialization).
template<>
inline file_t SQLStatementIterator<file_t>::operator*() const {
    return file_t(
            Query_->get<FIL_ID>(0),
            Query_->get<FIL_PT>(1));
}

/// Deference operator (`const function_t` specialization).
template<>
inline function_t SQLStatementIterator<function_t>::operator*() const {
    return function_t(
            Query_->get<FUN_ID>(0),
            Query_->get<FUN_NM>(1),
            Query_->get<FUN_SG>(2),
            Query_->get<FIL_ID>(3),
            Query_->get<LIN_NO>(4),
            Query_->get<FunctionType>(6));
}

/// Deference operator (`const instruction_t` specialization).
template<>
inline instruction_t SQLStatementIterator<instruction_t>::operator*() const {
    return instruction_t(
            Query_->get<INS_ID>(0),
            Query_->get<SEG_ID>(1),
            Query_->get<InstructionType>(2),
            Query_->get<LIN_NO>(3));
}

/// @brief Deference operator (`const loopExecution_t` specialization).
/// @todo Define types for `parent` and `duration`
template<>
inline loopExecution_t SQLStatementIterator<loopExecution_t>::operator*() const {
    return loopExecution_t(
            Query_->get<LOE_ID>(0),
            Query_->get<LOE_ID>(1),
            // TODO Define types for these variables!
            Query_->get<int>(2),
            Query_->get<int>(3));
}

/// Deference operator (`const loopIteration_t` specialization).
template<>
inline loopIteration_t SQLStatementIterator<loopIteration_t>::operator*() const {
    return loopIteration_t(
            Query_->get<LOI_ID>(0),
            Query_->get<LOE_ID>(1),
            Query_->get<LOI_ID>(2));
}

/// Deference operator (`const loop_t` specialization).
template<>
inline loop_t SQLStatementIterator<loop_t>::operator*() const {
    return loop_t(
            Query_->get<LOP_ID>(0),
            Query_->get<LIN_NO>(1));
}

/// Deference operator (`const reference_t` specialization).
template<>
inline reference_t SQLStatementIterator<reference_t>::operator*() const {
    return reference_t(
            Query_->get<REF_ID>(0),
            Query_->get<REF_SIZE>(1),
            Query_->get<ReferenceType>(2),
            Query_->get<REF_NAME>(3),
            Query_->get<INS_ID>(4));
}

/// Deference operator (`const segment_t` specialization).
template<>
inline segment_t SQLStatementIterator<segment_t>::operator*() const {
    return segment_t(
            Query_->get<SEG_ID>(0),
            Query_->get<CAL_ID>(1),
            Query_->get<SegmentType>(2),
            Query_->get<LOI_ID>(3));
}

/// Deference operator (`const thread_t` specialization).
template<>
inline thread_t SQLStatementIterator<thread_t>::operator*() const {
    return thread_t(
            Query_->get<TRD_ID>(0),
            Query_->get<TIME_STRING>(1),
            Query_->get<TIME_STRING>(2),
            Query_->get<TIME>(3),
            Query_->get<TIME>(4),
            Query_->get<INS_ID>(5),
            Query_->get<INS_ID>(6),
            Query_->get<TRD_ID>(7),
            Query_->get<PID>(8),
            Query_->get<CAL_ID>(9));
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
