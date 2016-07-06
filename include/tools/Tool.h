/*
 * Observer.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

class Event;

class Tool {
public:
virtual void Access(const Event* e) = 0;
virtual void Acquire(const Event* e) = 0;
virtual void Call(const Event* e) = 0;
virtual void Join(const Event* e) = 0;
virtual void NewThread(const Event* e) = 0;
virtual void Release(const Event* e) = 0;
virtual void Return(const Event* e) = 0;
virtual void ThreadEnd(const Event* e) = 0;

virtual ~Tool() {};
};

#endif /* OBSERVER_H_ */
