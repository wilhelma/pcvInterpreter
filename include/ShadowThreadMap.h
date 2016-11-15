/*
 * ShadowThreadMap.h
 *
 *  Created on: Sep 04, 2014
 *      Author: wilhelma
 */

#ifndef THREAD_MGR_H_
#define THREAD_MGR_H_

#include "ShadowMap.h"
#include "ShadowThread.h"
#include "Types.h"

/// @brief Maps a thread ID to the corresponding ShadowThread.
class ShadowThreadMap : public ShadowMap<TRD_ID, ShadowThread> {};

#endif
