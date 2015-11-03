/*
* DBDataModel.h
*
*  Created on: Sep 2, 2014
*      Author: wilhelma
*/

#ifndef DBDATAMODEL_H_
#define DBDATAMODEL_H_

#include <cstring>
#include <string>
#include "DataModel.h"

// constants---------------------------------------------------------------
static const unsigned ITYPELEN = 10;
static const unsigned TIMELEN = 12;
static const unsigned CALLIDLEN = 50;
static const unsigned REFIDLEN = 100;
static const unsigned MEMSTATELEN = 10;
static const unsigned FILENAMELEN = 256;
static const unsigned FILEPATHLEN = 1024;
static const unsigned SIGNATURELEN = 10000;
static const unsigned FNTYPELEN = 20;
static const unsigned REFNAMELEN = 2000;
static const unsigned SEGTYPELEN = 2;

// types-------------------------------------------------------------------
typedef unsigned 		INS_ID;		// instruction id
typedef unsigned 		SEG_ID;		// segment id
typedef unsigned 		ACC_ID;		// access id
typedef unsigned		REF_ID;		// reference id
typedef std::string 	CAL_ID;		// call id
typedef unsigned		FUN_ID;		// function id
typedef unsigned 		FIL_ID;		// file id
typedef std::string		REF_NO;		// reference no
typedef int				REF_ADDR;	// reference address
typedef unsigned		REF_SIZE;	// reference size
typedef std::string		REF_NAME;	// reference name
typedef char			REF_MTYP;	// memory type
typedef char			ACC_TYP;	// access type
typedef char*			FUN_TYP;	// function type
typedef unsigned		TRD_ID;		// thread id (table)
typedef unsigned		TRD_TID;	// thread id (system)

typedef struct access_t {
	INS_ID instruction_id;
	int position;
	char reference_id[REFIDLEN];
	ACC_TYP access_type;
	char memory_state[MEMSTATELEN];

	access_t(int instructionID,
			 int pos,
			 const unsigned char *referenceID,
			 const unsigned char *accessType,
			 const unsigned char *memoryState)
		: instruction_id(instructionID), position(pos),
		  access_type(*accessType)
	{
		strncpy(reference_id, (const char*)referenceID, REFIDLEN);
		strncpy(memory_state, (const char*)memoryState, MEMSTATELEN);
	}

	static Access::type getAccessType(ACC_TYP accType) {
		switch (accType) {
		case 'R':
			return Access::READ;
		case 'W':
			return Access::WRITE;
		default:
			return Access::READWRITE;
		}
	}
} access_t;

typedef struct call_t {
	int process_id;
	int thread_id;
	FUN_ID function_id;
	INS_ID instruction_id;
	char start_time[TIMELEN];
	char end_time[TIMELEN];

	call_t(int processID,
		   int threadID,
		   int functionID,
		   int instructionID,
		   const unsigned char *startTime,
		   const unsigned char *endTime)
		: process_id(processID), thread_id(threadID),
		  function_id(functionID), instruction_id(instructionID)
	{
		strncpy(start_time, (const char*)startTime, TIMELEN);
		strncpy(end_time, (const char*)endTime, TIMELEN);
	}
} call_t;

typedef struct file_t {
	char file_name[FILENAMELEN], file_path[FILEPATHLEN];

	file_t(const unsigned char *fileName,
		   const unsigned char *filePath)
	{
		strncpy(file_name, (const char*)fileName, FILENAMELEN);
		strncpy(file_path, (const char*)filePath, FILEPATHLEN);
	}
} file_t;

typedef struct function_t {
	char signature[SIGNATURELEN], type[FNTYPELEN];
	FIL_ID file_id;

	function_t(const unsigned char *fnSignature,
			   const unsigned char *fnType,
			   int fileId)
		: file_id(fileId)
	{
		strncpy(signature, (const char*)fnSignature, SIGNATURELEN);
		strncpy(type, (const char*)fnType, FNTYPELEN);
	}

	static Function::type getFunctionType(const FUN_TYP fnType) {
		if (strcmp( fnType, "METHOD") == 0)
			return Function::METHOD;
		else if (strcmp( fnType, "FUNCTION") == 0)
			return Function::FUNCTION;
		else if (strcmp( fnType, "ALLOC" ) == 0)
			return Function::ALLOC;
		else if (strcmp( fnType, "FREE" ) == 0)
			return Function::FREE;
		else if (strcmp( fnType, "ACQUIRE" ) == 0)
			return Function::ACQUIRE;
		else if (strcmp( fnType, "RELEASE" ) == 0)
			return Function::RELEASE;
		else if (strcmp( fnType, "FORK" ) == 0)
			return Function::FORK;
		else if (strcmp( fnType, "JOIN" ) == 0)
			return Function::JOIN;

		return Function::OTHER;
	}
} function_t;

typedef struct instruction_t {

	INS_ID instruction_id;
	SEG_ID segment_id;
	char instruction_type[ITYPELEN];
	int line_number;

	instruction_t(INS_ID instructionId,
				  int segmentId,
				  const unsigned char *instructionType,
				  int lineNumber) 
				  : instruction_id(instructionId), segment_id(segmentId),
					line_number(lineNumber)
	{
		strncpy(instruction_type, (const char*)instructionType, ITYPELEN);
	}

	static Instruction::type getInstructionType(FUN_TYP fnType) {
		if (strcmp( fnType, "CALL") == 0)
			return Instruction::CALL;
		else if (strcmp( fnType, "ACCESS" ) == 0)
			return Instruction::MEMACCESS;
		else if (strcmp( fnType, "CSENTER" ) == 0)
			return Instruction::ACQUIRE;
		else if (strcmp( fnType, "CSLEAVE" ) == 0)
			return Instruction::RELEASE;
		else
			return Instruction::OTHER;
	}
} instruction_t;

typedef struct reference_t {

	static const REF_MTYP STATIC = 'S';
	static const REF_MTYP HEAP = 'H';
	static const REF_MTYP GLOBAL = 'G';
	static const REF_MTYP LOCAL = 'L';

	char reference_id[REFIDLEN];
	REF_ID id;
	//REF_ADDR address;
	REF_SIZE size;
	REF_MTYP memory_type;
	char name[REFNAMELEN];
	int allocinstr;

	reference_t(const unsigned char *referenceId,
				int refId,
				//int refAddr,
				int refSize,
				const unsigned char *memoryType,
				const unsigned char *refName,
				int allocInstr)
		: id(refId), /*address(refAddr),*/ size(refSize), memory_type(*memoryType),
		  allocinstr(allocInstr)
	{
		strncpy(reference_id, (const char*)referenceId, REFIDLEN);
		strncpy(name, (const char*)refName, REFNAMELEN);
	}

} reference_t;

typedef struct segment_t {

	std::string call_id;
	int segment_no;
	char segment_type[SEGTYPELEN];
	int loop_pointer;

	segment_t(const unsigned char *callId,
			int segmentNo,
			const unsigned char *segmentType,
			int loopPointer)
		: call_id((const char*)callId), segment_no(segmentNo),
		  loop_pointer(loopPointer)
	{
		strncpy(segment_type, (const char*)segmentType, SEGTYPELEN);
	}

} segment_t;

typedef struct thread_t {

	TRD_ID id;
	INS_ID instruction_id;
	TRD_TID parent_thread_id;
	TRD_TID	child_thread_id;

	thread_t(int id,
			 int instructionID,
			 int parentThreadId,
			 int childThreadId)
		: id(id), instruction_id(instructionID), 
		parent_thread_id(parentThreadId), child_thread_id(childThreadId) {}

} thread_t;


#endif /* DBDATAMODEL_H_ */
