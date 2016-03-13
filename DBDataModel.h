/**
 * @file DBDataModel.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef DBDATAMODEL_H_
#define DBDATAMODEL_H_

#include <cstring>
#include <string>
#include <iostream>
#include "DataModel.h"

// constants---------------------------------------------------------------
static const unsigned FILENAMELEN = 256;
static const unsigned FILEPATHLEN = 1024;
static const unsigned SIGNATURELEN = 10000;
static const unsigned REFNAMELEN = 2000;

// types-------------------------------------------------------------------
typedef unsigned        ID;         //!< @brief  SQL ID field
typedef unsigned 		INS_ID;		//!< @brief  Instruction id
typedef unsigned        INS_TYP;    //!< @brief  Instruction type
typedef unsigned 		SEG_ID;		//!< @brief  Segment id
typedef unsigned        SEG_NO;     //!< @brief  segment number
typedef unsigned        SEG_TYP;    //!< @brief  segment type
typedef unsigned 		ACC_ID;		//!< @brief  Access id
typedef unsigned		REF_ID;		//!< @brief  Reference id
typedef unsigned        CAL_ID;		//!< @brief  Call id
typedef unsigned		FUN_ID;		//!< @brief  Function id
typedef unsigned 		FIL_ID;		//!< @brief  File id
typedef unsigned        LIN_NO;     //!< @brief  Line number
typedef std::string		REF_NO;		//!< @brief  Reference no
typedef int				REF_ADDR;	//!< @brief  Reference address
typedef unsigned		REF_SIZE;	//!< @brief  Reference size
typedef std::string		REF_NAME;	//!< @brief  Reference name
typedef unsigned		REF_MTYP;	//!< @brief  Memory type
typedef unsigned		ACC_TYP;	//!< @brief  Access type
typedef unsigned		FUN_TYP;	//!< @brief  Function type
typedef unsigned		TRD_ID;		//!< @brief  Thread id (table)
typedef unsigned		TRD_TID;	//!< @brief  Thread id (system)
typedef unsigned        MEM_ST;     //!< @brief  Memory state
typedef unsigned        LOP_ID;     //!< @brief  Loop ID
typedef unsigned        LOE_ID;     //!< @brief  loopExecution id
typedef unsigned		LOI_ID;	    //!< @brief  loopIteration id (system)
typedef clock_t         TIME_TYP;   //!< @brief  Time 

typedef struct access_t {
	ID sql_id;
	INS_ID instruction_id;
	int position;
	REF_ID reference_id;
	ACC_TYP access_type;
	MEM_ST memory_state;

	explicit
	access_t(ID sqlID,
			 INS_ID instructionID,
			 int pos,
			 REF_ID referenceID,
			 ACC_TYP accessType,
			 MEM_ST memoryState)
		: sql_id(sqlID), instruction_id(instructionID), position(pos),
		  reference_id(referenceID), access_type(accessType),
		  memory_state(memoryState)
	{
	}
} access_t;

// Try to make a new struct for call_t as the database format has changed
typedef struct call_t {
	ID sql_id;
	TRD_ID thread_id;
	FUN_ID function_id;
	INS_ID instruction_id;
	TIME_TYP start_time;
	TIME_TYP end_time;

	explicit
	call_t(ID sqlID,
		   TRD_ID threadID,
		   FUN_ID functionID,
		   INS_ID instructionID,
		   TIME_TYP startTime,
		   TIME_TYP endTime)
		: sql_id(sqlID), thread_id(threadID),
		  function_id(functionID), instruction_id(instructionID),
		  start_time(startTime), end_time(endTime)
	{
	}
} call_t;

/**
 * @brief Record to process the `File` table in the database.
 */
typedef struct file_t {
	ID sql_id;
	char file_path[FILEPATHLEN];
	// this is retrieved from the file_path[]
	char file_name[FILENAMELEN];

	explicit
	file_t(ID sqlID,
		   const unsigned char *filePath)
        : sql_id(sqlID)
	{
		strncpy(file_path, (const char*)filePath, FILEPATHLEN);
		retrieveFileName( filePath );
	}

	/**
	 * @brief Tries to retrieve the file name from the file path.
	 */
	int retrieveFileName ( const unsigned char *filePath ) {
		int lastFound = -1;

		/**
		 * Looks for '/' or '\' till the end of the string.
		 */
		size_t i = 0;
		const char directorySeparator = '/'; // use '\' for Windows!
		while ( filePath[i] != '\0' && i < FILEPATHLEN ) {
			if ( filePath[i] == directorySeparator )
				lastFound = i;

			i++;
		}
		
		// check 
		if( lastFound != -1 ) {
			/**
			 * Copyes what comes after the last directory delimiter into
			 * `file_name[]`.
			 *
			 * @attention I assume `i - lastFound <= FILENAMELEN` */
			strncpy( file_name, (const char*) &filePath[lastFound+1], i - lastFound );
			return 0;
		} else {
			/**
			 * If no '/' (or '\') is found in the path, then the whole 
			 * string is taken as the file name. 
			 */
			strncpy( file_name, (const char*) &filePath[lastFound+1], i - lastFound );
			return 1;
		}
	}
} file_t;

typedef struct function_t {
	char signature[SIGNATURELEN];
	FUN_TYP type;
	FIL_ID file_id;
	LIN_NO line_number;

	explicit
	function_t(const unsigned char *fnSignature,
			   FUN_TYP fnType,
			   FIL_ID fileId,
			   LIN_NO lineNumber)
		: type(fnType), file_id(fileId), line_number(lineNumber)
	{
		strncpy(signature, (const char*)fnSignature, SIGNATURELEN);
		std::cout << " >> " << signature << std::endl;
		getFunctionType(fnType);
	}

	static void printFunctionType(unsigned short shift) {
		switch(shift) {
			case 0:
				std::cout << "ENTRY_ROUTINE" << std::endl;
				break;
			case 1:
				std::cout << "EXIT_ROUTINE"  << std::endl;
				break;
			case 2:
				std::cout << "FUNCTION"  << std::endl;
				break;
			case 3:
				std::cout << "METHOD"  << std::endl;
				break;
			case 4:
				std::cout << "FREE"  << std::endl;
				break;
			case 5:
				std::cout << "FORK"  << std::endl;
				break;
			case 6:
				std::cout << "JOIN"  << std::endl;
				break;
			case 7:
				std::cout << "ACQUIRE"  << std::endl;
				break;
			case 8:
				std::cout << "RELEASE"  << std::endl;
				break;
			case 9:
				std::cout << "EXTERNAL"  << std::endl;
				break;
			case 10:
				std::cout << "ALLOC"  << std::endl;
				break;
			case 11:
				std::cout << "BARRIER"  << std::endl;
				break;
			case 12:
				std::cout << "WAIT"  << std::endl;
				break;
			case 13:
				std::cout << "SIGNAL_SINGLE"  << std::endl;
				break;
			case 14:
				std::cout << "SIGNAL_BROADCAST"  << std::endl;
				break;
		}
	}

	/**
	 * @brief Decodes the function type
	 * @todo Return a proper thing!!
	 */
	static Function::type getFunctionType(FUN_TYP fnType) {

		// shift right functionType till there is no information left
		unsigned int counter = 0;
		while( fnType != 0 ){
			if( fnType & 1 ) {
				std::cout << "   -- has type " << (1 << counter) << std::endl;
				std::cout << "    -> ";
				printFunctionType( counter );
			}

			counter ++;
			// shift right by one
			fnType >>= 1;
		}
		return Function::OTHER;
	}
} function_t;

typedef struct instruction_t {
	INS_ID instruction_id;
	SEG_ID segment_id;
	INS_TYP instruction_type;
	LIN_NO line_number;

	explicit
	instruction_t(INS_ID instructionId,
				  SEG_ID segmentId,
				  INS_TYP instructionType,
				  LIN_NO lineNumber) 
				  : instruction_id(instructionId), segment_id(segmentId),
                  instruction_type(instructionType), line_number(lineNumber)
	{
	}
} instruction_t;

typedef struct loop_t {
	unsigned line_number;

	explicit
	loop_t(int lineNumber)
		: line_number(lineNumber)
	{}

} loop_t;

typedef struct loopExecution_t {
	unsigned loop_id;
	unsigned parent_iteration;
	unsigned loop_duration;

	explicit
	loopExecution_t(int loopID,
			        int parentIteration,
					int loopDuration)
		            : loop_id(loopID), parent_iteration(parentIteration),
					  loop_duration(loopDuration)
	{}

} loopExecution_t;

typedef struct loopIteration_t {
	unsigned loop_execution;
	unsigned loop_iteration;

	explicit
	loopIteration_t(int loopExecution,
			        int loopIteration)
		            : loop_execution(loopExecution),
					  loop_iteration(loopIteration)
	{}

} loopIteration_t;

typedef struct reference_t {
	REF_ID id;
	//REF_ADDR address;
	REF_SIZE size;
	REF_MTYP memory_type;
	char name[REFNAMELEN];
	int allocinstr;

	explicit
	reference_t(int refId,
				int refSize,
				int memoryType,
				const unsigned char *refName,
				int allocInstr)
		: id(refId), size(refSize), memory_type(memoryType),
		  allocinstr(allocInstr)
	{
		strncpy(name, (const char*)refName, REFNAMELEN);
	}

} reference_t;

typedef struct segment_t {
	CAL_ID call_id;
	SEG_TYP segment_type;
	int loop_pointer;

	explicit
	segment_t(CAL_ID callId,
			  SEG_TYP segmentType,
			  int loopPointer)
		: call_id(callId), segment_type(segmentType),
		  loop_pointer(loopPointer)
	{
	}

} segment_t;

typedef struct thread_t {
	TRD_ID id;
	int process_id;
	INS_ID instruction_id;
	TRD_TID	child_thread_id;
	TRD_TID parent_thread_id;

	explicit
	thread_t(int id,
			 int processId,
			 int instructionId,
			 int childThreadId,
			 int parentThreadId)
		: id(id), process_id(processId), instruction_id(instructionId), 
		  child_thread_id(childThreadId), parent_thread_id(parentThreadId) {}

} thread_t;


#endif /* DBDATAMODEL_H_ */
