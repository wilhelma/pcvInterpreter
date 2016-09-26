/**
 *
 *    @file  ThreadSegment.cpp
 *   @brief  Implementation file for the class `ThreadSegment`
 *
 *    @date  09/26/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "ThreadSegment.h"

ThreadSegment::ThreadSegment(){}

explicit ThreadSegment(TIME strt) : stop(static_cast<TIME>(0)), span(static_cast<TIME>(0)) {

	start = strt;
}

ThreadSegment::~ThreadSegment() {}

ThreadSegment::end_segment(TIME stp, TIME spn) {
	stop = stp;
	span = spn;
}