/**
 *
 *    @file  DatabaseException.h
 *   @brief  
 *
 *    @date  10/24/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  DATABASE_EXCEPTION_H_
#define  DATABASE_EXCEPTION_H_

#include <exception>
#include <string>

/// @ingroup database
/// @brief Esception class for the Database class.
class DatabaseException : public std::exception {
public:
    /// Constructor.
    /// @param what_arg  String descripting exception
    /// @param func_name Name of the function originating the exception
    DatabaseException(std::string&& what_arg, std::string&& func_name):
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
