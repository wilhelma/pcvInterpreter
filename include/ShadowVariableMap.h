/**
 *
 *    @file  ShadowVarMap.h
 *   @brief  
 *
 *    @date  11/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SHADOW_VARIABLE_MAP_H_
#define  SHADOW_VARIABLE_MAP_H_

#include "ShadowMap.h"
#include "Types.h"

#include "Access.h" // contains AccessType!

#include "fwd/AccessInfo.h"
#include "fwd/Reference.h"
#include "fwd/ShadowVariable.h"

/// @ingroup shadows
/// @brief Maps a reference ID to the corresponding ShadowVariable.
class ShadowVariableMap : public ShadowMap<REF_ID, ShadowVariable> {};

#endif
