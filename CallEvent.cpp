/*
 * CallEvent.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#include "Event.h"

const CallInfo* CallEvent::getCallInfo() const
{
	return _info;
}
