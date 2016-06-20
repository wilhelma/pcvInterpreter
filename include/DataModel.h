/**
 *  @file DataModel.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef DATAMODEL_H_
#define DATAMODEL_H_

#include <string>
//#include "DBDataModel.h"

typedef int MemAddress;
//typedef TRD_ID ThreadId; // unsigned
typedef unsigned RefNo; // unsigned



/*! @brief The type of the function */
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

/*! @brief The type of the instruction */
enum class InstructionType {
    CALL   = 0,
    ACCESS = 1,
    ALLOC  = 2,
    FREE   = 3,
    FORK   = 4,
    JOIN   = 5
};

/*! @brief The type of the reference */
enum class ReferenceType {
    STACK     = 1,
    HEAP      = (1 << 1),
    STATICVAR = (1 << 2),
    GLOBAL    = (1 << 3)
};

/*! @brief The type of the segment */
enum class SegmentType : bool {
    REGION = 0,
    LOOP   = 1
};

#endif /* DATAMODEL_H_ */
