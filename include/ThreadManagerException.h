/**
 *
 *    @file  ThreadManagerException.h
 *   @brief  
 *
 *    @date  11/07/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  THREAD_MANAGER_EXCEPTION_H_
#define  THREAD_MANAGER_EXCEPTION_H_

#include <exception>
#include <string>

/// @brief Exception class for the ThreadMgr class.
class ThreadManagerException : public std::exception {
public:
    /// Constructor.
    /// @param what_arg  String descripting the exception.
    /// @param func_name Name of the function originating the exception.
    ThreadManagerException(std::string&& what_arg, std::string&& func_name) :
        std::exception(),
        What_(what_arg + (func_name.empty() ? "" : (" from " + func_name)))
    {}

    /// Exception description.
    const char* what() const noexcept final
    { return What_.c_str(); }

protected:
    /// The exception message: ''<_error_> from <_function_>''.
    const std::string What_;
};

#endif
