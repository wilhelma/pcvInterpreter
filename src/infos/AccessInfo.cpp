/**
 *
 *    @file  AccessInfo.cpp
 *   @brief  
 *
 *    @date  11/21/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "AccessInfo.h"

#include "Access.h"
#include "Reference.h"
#include "ShadowVariable.h"
#include "ShadowVariableMap.h"
#include "Types.h"

const REF_ID& AccessInfo::variableId() const noexcept
{ return Variable_->first; }

const AccessType AccessInfo::accessType() const noexcept
{ return AccessType_; }

const ReferenceType AccessInfo::referenceType() const noexcept
{ return Variable_->second->type(); }

const REF_SIZE& AccessInfo::size() const noexcept
{ return Variable_->second->size(); }

const REF_NAME& AccessInfo::name() const noexcept
{ return Variable_->second->name(); }
