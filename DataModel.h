/*
 * DataModel.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef DATAMODEL_H_
#define DATAMODEL_H_

#include <string>

/*----------------------------------------------------------------------------
 * Types
 ----------------------------------------------------------------------------*/
typedef int MemAddress;
typedef unsigned ThreadId;
typedef unsigned int RefId;
typedef unsigned RefNo;

/*----------------------------------------------------------------------------
 * Instruction
 ----------------------------------------------------------------------------*/
typedef struct {
	typedef enum { MEMACCESS, 	// memory access instruction
				   CALL,		// call instruction
				   ALLOC,		// memory allocation instruction
				   FREE,		// instruction to free memory
				   ACQUIRE,		// instruction to acquire a lock
				   RELEASE,		// release lock instruction
				   FORK,		// create a thread instruction
				   JOIN,		// join a thread instruction
				   OTHER		// non-handled instruction
				 } type;
} Instruction;
							   
/*----------------------------------------------------------------------------
 * Access
 ----------------------------------------------------------------------------*/
typedef struct {
	typedef enum { READ,		// read access
				   WRITE,		// write access
				   READWRITE	// read and write access
				 } type;
} Access;

///*----------------------------------------------------------------------------
// * Function
// ----------------------------------------------------------------------------*/
//typedef struct {
//	typedef enum { FUNCTION,		// user function
//				   METHOD,			// user method
//				   ALLOC,			// allocation function
//				   FREE,			// free memory function
//				   ACQUIRE,			// acquire lock function
//				   RELEASE,			// release lock function
//				   FORK,			// fork thread function
//				   JOIN,			// join thread function
//				   OTHER			// non-handled function
//				 } type;
//} Function;
//
///*! @brief The type of the access */
//enum class Type {
//    READ  = 1,
//    WRITE = 2
//};
//
///*! @brief The state of the access */
//enum class State {
//    INIT              = 0,
//    EXCLUSIVE         = 1,
//    READ_SHARED       = 2,
//    READ_WRITE_SHARED = 3
//};

/*!
 * @brief The type of the function
 * @todo Find a smarter way to have strongly typed enums
 * than having them wrapped into `struct`s
 */
typedef struct {
//	typedef enum class Type : uint32_t  {
	typedef enum {
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
//		OTHER            = (1 << 15)  // XXX back-compatibility
	} type;
} Function;

///*! @brief The type of the instruction */
//enum class Type {
//    CALL   = 0,
//    ACCESS = 1,
//    ALLOC  = 2,
//    FREE   = 3,
//    FORK   = 4,
//    JOIN   = 5
//};
//
///*! @brief The type of the reference */
//enum class Type {
//    STACK     = 1,
//    HEAP      = (1 << 1),
//    STATICVAR = (1 << 2),
//    GLOBAL    = (1 << 3)
//};
//
///*! @brief The type of the segment */
//enum class Type {
//    REGION = 0,
//    LOOP   = 1
//};

#endif /* DATAMODEL_H_ */
