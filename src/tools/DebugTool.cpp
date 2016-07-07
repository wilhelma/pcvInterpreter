/**
 *
 *    @file  DebugTool.cpp
 *   @brief  Implementation file for the class `DebugTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <utility>
#include "DebugTool.h"


DebugTool::DebugTool() {

}

DebugTool::~DebugTool() {

}

void DebugTool::create(const Event* e) {

  printf("NEW THREAD EVENT \n");
}

void DebugTool::join(const Event* e) {

  printf("JOIN EVENT \n");
}

void DebugTool::call(const Event* e) {

  printf("CALL EVENT \n");
}

void DebugTool::returnOfCalled(const Event* e) {

  printf("RETURN EVENT \n");
}

void DebugTool::threadEnd(const Event* e) {

  printf("THREAD END EVENT \n");
}

void DebugTool::acquire(const Event* e) {

  printf("ACQUIRE EVENT \n");
}

void DebugTool::release(const Event* e) {

  printf("RELEASE EVENT \n");
}

void DebugTool::access(const Event* e) {

  printf("ACCESS EVENT \n");
}




