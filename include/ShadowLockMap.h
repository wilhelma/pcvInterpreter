/*
 * ShadowLockMap.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef SHADOW_LOCK_MAP_H_
#define SHADOW_LOCK_MAP_H_

#include "ShadowMap.h"
#include "Types.h"

#include "fwd/ShadowLock.h"

/// @brief Maps a reference ID to the corresponding ShadowLock.
class ShadowLockMap : public ShadowMap<REF_ID, ShadowLock> {};

#endif
