/**
 *
 *    @file  SQLException.h
 *   @brief  
 *
 *    @date  07/21/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SQL_EXCEPTION_H_
#define  SQL_EXCEPTION_H_

#include <exception>
#include <string>

/// @ingroup database
/// @brief Exception class for SQL-related objects.
class SQLException: public std::exception {
public:
    /// Constructor.
    /// @param what_arg  String descripting exception
    /// @param func_name Name of the function originating the exception
    SQLException(std::string&& what_arg, std::string&& func_name):
        std::exception(),
        What_(what_arg + (func_name.empty() ? "" : (" from " + func_name)))
    {}

    /// Exception description.
    virtual const char* what() const noexcept override final {
        return What_.c_str();
    }

protected:
    /// The exeption message: ''<_error_> from <_function_>''.
    const std::string What_;
};

#endif
