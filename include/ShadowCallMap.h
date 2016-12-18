/**
 *
 *    @file  ShadowCallMap.h
 *   @brief  
 *
 *    @date  12/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  SHADOW_CALL_MAP_H_
#define  SHADOW_CALL_MAP_H_

#include "fwd/ShadowCall.h"

#include "ShadowMap.h"
#include "Types.h"

/// @ingroup shadows
/// @brief Maps a CAL_ID to the corresponding ShadowCal
class ShadowCallMap final : public ShadowMap<CAL_ID, ShadowCall> {};

#endif
