/*
 * JoinEvent.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#include "Event.h"

const JoinInfo* JoinEvent::getJoinInfo() const
{
	return _info;
}
