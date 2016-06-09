/*
 * NewThreadEvent.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#include "Event.h"

const NewThreadInfo* NewThreadEvent::getNewThreadInfo() const
{
	return _info;
}
