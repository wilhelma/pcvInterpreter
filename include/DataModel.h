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






/*! @brief The type of the segment */
enum class SegmentType : bool {
    REGION = 0,
    LOOP   = 1
};

#endif /* DATAMODEL_H_ */
