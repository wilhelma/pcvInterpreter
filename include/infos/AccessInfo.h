/**
 *
 *    @file  AccessInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_INFO_H_
#define  ACCESS_INFO_H_

#include "fwd/ShadowVar.h"

#include "Access.h"
#include "Types.h"

struct AccessInfo {
	AccessType type;
	INS_ID instructionID;
	const ShadowVar *var;

	explicit
	AccessInfo(AccessType Type, const ShadowVar *Var, const INS_ID& instructionID) noexcept
		: type(Type), instructionID(instructionID), var(Var) {}
};

#endif
