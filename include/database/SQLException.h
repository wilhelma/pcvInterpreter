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

/// Exception class for SQL-related objects.
class SQLException: public std::exception {
public:
    /// Constructor.
    /// @param what_arg  String descripting exception
    /// @param func_name Name of the function originating the exception
    SQLException(const std::string& what_arg, const std::string& func_name):
//      std::exception(), What_(what_arg), WhatReturn_(nullptr), Function_(func_name)
        std::exception(), WhatReturn_((what_arg + (func_name.empty() ? "" : (" from " + func_name))).c_str())
    {}

    /// @return A `const char*` with the exception description
    virtual const char* what() const noexcept override final {
//      WhatReturn_ = (What_ + (Function_.empty() ? "" : (" from " + Function_))).c_str();
        return WhatReturn_;
    }

protected:
//  std::string What_;
//  std::string Function_;

    /// In order to prevent what() to deallocate the memory before it's accessed.
    const char* WhatReturn_;
};

#endif
