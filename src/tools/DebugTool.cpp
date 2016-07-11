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
  static int count = 0; count++;
  printf("NEW THREAD EVENT %d\n", count);
}

void DebugTool::join(const Event* e) {
  static int count = 0; count++;
  printf("JOIN EVENT %d\n", count);
}

void DebugTool::call(const Event* e) {
  static int count = 0; count++;
  printf("CALL EVENT %d\n", count);
}

void DebugTool::returnOfCalled(const Event* e) {
  static int count = 0; count++;
  printf("RETURN EVENT %d\n", count);
}

void DebugTool::threadEnd(const Event* e) {
  static int count = 0; count++;
  printf("THREAD END EVENT %d\n", count);
}

void DebugTool::acquire(const Event* e) {
  static int count = 0; count++;
  printf("ACQUIRE EVENT %d\n", count);
}

void DebugTool::release(const Event* e) {
  static int count = 0; count++;
  printf("RELEASE EVENT %d\n", count);
}

void DebugTool::access(const Event* e) {
  static int count = 0; count++;
  printf("ACCESS EVENT %d\n", count);
}




