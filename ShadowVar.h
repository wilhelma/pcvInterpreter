/*
 * ShadowVar.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef SHADOWVAR_H_
#define SHADOWVAR_H_

#include "DBDataModel.h"


/******************************************************************************
 * Shadow variable
 *****************************************************************************/
class ShadowVar {
public:
	typedef enum { STACK 	= 0x1,
				   GLOBAL	= 0x2,
				   HEAP		= 0x4,
				   STATIC 	= 0x8,
				   ERROR	= 0x0
				 } VarType;

	ShadowVar(const VarType Type,
			  const REF_ID Id,
			  //const REF_ADDR Addr,
			  const REF_SIZE Size,
			  const REF_NAME Name)
				 : ptr(nullptr), type(Type), id(Id), //addr(Addr),
				   size(Size), name(Name) {}

	void* ptr;

	const VarType type;
	const REF_ID id;
	//const REF_ADDR addr;
	const REF_SIZE size;
	const REF_NAME name;

private:
	// prevent generated functions
	ShadowVar(const ShadowVar&);
	ShadowVar& operator=(const ShadowVar&);
};

#endif /* SHADOWVAR_H_ */
