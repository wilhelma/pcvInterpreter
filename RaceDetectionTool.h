/*
 * RaceDetectionTool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef RACEDETECTIONTOOL_H_
#define RACEDETECTIONTOOL_H_

#include "Tool.h"

class RaceDetectionTool : public Tool {
public:
	RaceDetectionTool();
	void create(const Event* e) override;
	void acquire(const Event* e) override;
	void release(const Event* e) override;
	void access(const Event* e) override;
	void call(const Event* e) override;

private:
	// prevent generated functions
	RaceDetectionTool(const RaceDetectionTool&);
	RaceDetectionTool& operator=(const RaceDetectionTool&);
};



#endif /* RACEDETECTIONTOOL_H_ */
