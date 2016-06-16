/**
 *
 *    @file  Access.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_H_
#define  ACCESS_H_

#include "DataModel.h"
#include "Types.h"

typedef struct access_t {
    ACC_ID sql_id;
    INS_ID instruction_id;
    POS position;
    REF_ID reference_id;
    AccessType access_type;
    AccessState memory_state;

    explicit
    access_t(ACC_ID sqlID,
             INS_ID instructionID,
             POS pos,
             REF_ID referenceID,
             AccessType accessType,
             AccessState memoryState)
        : sql_id(sqlID), instruction_id(instructionID), position(pos),
          reference_id(referenceID), access_type(accessType),
          memory_state(memoryState)
    {
    }
} access_t;

#endif
