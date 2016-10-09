/*
 * ShadowVar.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_VAR_H_
#define SHADOW_VAR_H_

#include "Reference.h"
#include "Types.h"

/// @ingroup shadows
/// @todo Document this!
struct ShadowVar {
	/// @bug Dangling pointer! It points nowhere!
	void* ptr;

	/// The variable type (i.e. `STACK`, `HEAP`...).
	const ReferenceType type;
	/// @todo Document this!
	const REF_ID id;
	/// Variable size.
	const REF_SIZE size;
	/// Variable name.
	const REF_NAME name;

	/// Constructor.
	explicit ShadowVar(const ReferenceType& Type, const REF_ID& Id,
			  const REF_SIZE& Size, const REF_NAME& Name) noexcept
				 : ptr(nullptr), type(Type), id(Id), //addr(Addr),
				   size(Size), name(Name) {}
	/// _Default_ destructor.
	~ShadowVar() = default;


	/// _Deleted_ copy constructor.
	ShadowVar(const ShadowVar&)            = delete;
	/// _Deleted_ move constructor.
	ShadowVar(ShadowVar&&)                 = delete;
	/// _Deleted_ copy assignment operator.
	ShadowVar& operator=(const ShadowVar&) = delete;
	/// _Deleted_ move assignment operator.
	ShadowVar& operator=(ShadowVar&&)      = delete;
};

#endif
