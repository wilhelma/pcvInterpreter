/**
 *
 *    @file  Reference.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  REFERENCE_H_
#define  REFERENCE_H_

#include "Types.h"

typedef struct reference_t {
    REF_ID id;
    //REF_ADDR address;
    REF_SIZE size;
    ReferenceType memory_type;
    REF_NAME name;
    INS_ID allocinstr;

    explicit
    reference_t(REF_ID refId,
                REF_SIZE refSize,
                ReferenceType memoryType,
                REF_NAME refName,
                INS_ID allocInstr)
        : id(refId), size(refSize), memory_type(memoryType),
          name(refName), allocinstr(allocInstr)
    {
    }

} reference_t;

#endif
