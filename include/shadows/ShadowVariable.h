/*
 * ShadowVariable.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_VAR_H_
#define SHADOW_VAR_H_

#include "Reference.h"
#include "Types.h"

/// @ingroup shadows
/// @brief Shadow abstraction for a variable.
class ShadowVariable {
public:
    /// @brief Constructor.
    /// @param var_reference The information about the accessed variable.
    explicit ShadowVariable(const reference_t& var_reference) noexcept
        : Type_(var_reference.memory_type),
          Size_(var_reference.size),
          Name_(var_reference.name)
        {}

	/// _Default_ destructor.
	~ShadowVariable() = default;


	/// _Deleted_ copy constructor.
	ShadowVariable(const ShadowVariable&) = delete;
	/// _Deleted_ move constructor.
	ShadowVariable(ShadowVariable&&)      = delete;
	/// _Deleted_ copy assignment operator.
	ShadowVariable& operator=(const ShadowVariable&) = delete;
	/// _Deleted_ move assignment operator.
	ShadowVariable& operator=(ShadowVariable&&)      = delete;

    /// Return the access type.
    const ReferenceType type() const noexcept
    { return Type_; }

    /// Return the variable size.
    const REF_SIZE& size() const noexcept
    { return Size_; }

    /// Return the variable name.
    const REF_NAME& name() const noexcept
    { return Name_; }

private:
	/// The variable type.
	const ReferenceType Type_;
	/// Variable size.
	const REF_SIZE Size_;
	/// Variable name.
	const REF_NAME Name_;
};

#endif
