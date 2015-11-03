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
typedef std::string RefNo;

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

/*----------------------------------------------------------------------------
 * Function
 ----------------------------------------------------------------------------*/
typedef struct {
	typedef enum { FUNCTION,		// user function
				   METHOD,			// user method
				   ALLOC,			// allocation function
				   FREE,			// free memory function
				   ACQUIRE,			// acquire lock function
				   RELEASE,			// release lock function
				   FORK,			// fork thread function
				   JOIN,			// join thread function
				   OTHER			// non-handled function
				 } type;
} Function;

#endif /* DATAMODEL_H_ */
