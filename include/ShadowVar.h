/*
 * ShadowVar.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef SHADOWVAR_H_
#define SHADOWVAR_H_

#include "Reference.h"
#include "Types.h"

/******************************************************************************
 * Shadow variable
 *****************************************************************************/
class ShadowVar {
public:
	ShadowVar(const ReferenceType Type,
			  const REF_ID Id,
			  const REF_SIZE Size,
			  const REF_NAME Name)
				 : ptr(nullptr), type(Type), id(Id), //addr(Addr),
				   size(Size), name(Name) {}

	void* ptr;

	const ReferenceType type;
	const REF_ID id;
	const REF_SIZE size;
	const REF_NAME name;

private:
	// prevent generated functions
	ShadowVar(const ShadowVar&);
	ShadowVar& operator=(const ShadowVar&);
};

#endif /* SHADOWVAR_H_ */
