/**
 *
 *    @file  ShadowVariable.cpp
 *   @brief  
 *
 *    @date  12/13/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ShadowVariable.h"

#include "Reference.h"
#include "Types.h"

ShadowVariable::ShadowVariable(const reference_t& var_reference) noexcept
: Type_(var_reference.memory_type),
  Size_(var_reference.size),
  Name_(var_reference.name)
{}
