/**
 *
 *    @file  Observer.h
 *   @brief  
 *
 *    @date  09/04/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */


#ifndef  FWD_OBSERVER_H_
#define  FWD_OBSERVER_H_

#include <list>

struct Observer;

/// @typedef ObserverList
/// @brief List of observers.
using ObserverList = std::list<Observer>;

#endif
