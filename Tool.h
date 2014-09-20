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
virtual void create(const Event* e) = 0;
virtual void join(const Event* e) = 0;
virtual void acquire(const Event* e) = 0;
virtual void release(const Event* e) = 0;
virtual void access(const Event* e) = 0;
virtual void call(const Event* e) = 0;

virtual ~Tool() {};
};

#endif /* OBSERVER_H_ */
