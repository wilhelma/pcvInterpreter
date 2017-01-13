/**
 *
 *    @file  FunctionType.h
 *   @brief  
 *
 *    @date  01/13/17
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_TYPE_H
#define  FUNCTION_TYPE_H

/// @ingroup types
/// @brief The type of the function.
/// @todo document this!
enum class FunctionType : uint32_t  {
	ENTRY_ROUTINE    = (1 << 0),  // 1
	EXIT_ROUTINE     = (1 << 1),  // 2
	FUNCTION         = (1 << 2),  // 4
	METHOD           = (1 << 3),  // 8
	FREE             = (1 << 4),  // ...
	FORK             = (1 << 5),
	JOIN             = (1 << 6),
	ACQUIRE          = (1 << 7),
	RELEASE          = (1 << 8),
	EXTERNAL         = (1 << 9),
	ALLOC            = (1 << 10),
	BARRIER          = (1 << 11),
	WAIT             = (1 << 12),
	SIGNAL_SINGLE    = (1 << 13),
	SIGNAL_BROADCAST = (1 << 14),
};

#endif
