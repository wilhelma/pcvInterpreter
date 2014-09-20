/*
 * RaceDetectionTool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef RACEDETECTIONTOOL_H_
#define RACEDETECTIONTOOL_H_

#include "Tool.h"

#include <iostream>
#include <array>
#include <set>
#include <map>
#include <memory>
#include "Interpreter.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "DataModel.h"
#include "DBDataModel.h"

#define THREADS 100

class RaceDetectionTool : public Tool {
public:
	RaceDetectionTool(const char* outFile);
	void create(const Event* e) override;
	void join(const Event* e) override;
	void acquire(const Event* e) override;
	void release(const Event* e) override;
	void access(const Event* e) override;
	void call(const Event* e) override;
	~RaceDetectionTool();

	typedef enum { WRITE_READ = 1,		// read after write (RAW)
				   READ_WRITE = 2,		// write after read (WAR)
				   WRITE_WRITE = 3		// write after write (WAW)
	} RaceType;

//	static Decoration<ShadowThread, Set> locksheld;
	
private:
	// Lock Set ---------------------------------------------------------------
	struct ShadowLockPtrComp {
		bool operator()(const ShadowLock* lhs, const ShadowLock* rhs) const {
			return lhs->lockId < rhs->lockId;
		}
	};
	
	typedef std::set<ShadowLock*, ShadowLockPtrComp> LockSet_;
	typedef std::map<const ShadowThread*, LockSet_> ThreadLockSet_;
	ThreadLockSet_ lockSet_;

	inline bool lsIsEmptySet(const LockSet_& lhs, const LockSet_& rhs) const;
	inline void lsIntersect(LockSet_& lhs, const LockSet_& rhs) const;

	// Vector Clock -----------------------------------------------------------
	typedef unsigned Clock_;
	typedef std::array<Clock_, THREADS> VectorClock_;
	typedef std::map<ThreadId, VectorClock_> ThreadVC_;

	ThreadVC_ threadVC_;

	inline void vcMerge(VectorClock_& lhs, const VectorClock_& rhs) const;
	inline bool vcLEQ(const Clock_& epoch, const VectorClock_& vc) const;

	
	typedef struct VarSet_ {
		Clock_ epoch;
		LockSet_ lockset;
		INS_ID instruction;

		VarSet_() : epoch(0), instruction(0) {}
	} VarSet_;

	typedef std::map<ThreadId, VarSet_> ThreadVarSet_;
	typedef std::map<RefId, ThreadVarSet_> ReadVarSet_;
	typedef std::map<RefId, VarSet_> WriteVarSet_;
	ReadVarSet_ readVarSet_;
	WriteVarSet_ writeVarSet_;

	// general ----------------------------------------------------------------
	const char* outFile_;

	typedef struct RaceEntry_ {
		const RaceType type;
		const INS_ID firstInstruction;
		const INS_ID secondInstruction;
		const REF_ID id;
		explicit RaceEntry_(RaceType type,
							INS_ID firstInstruction,
							INS_ID secondInstruction,
							REF_ID id)
		: 	type(type),
			firstInstruction(firstInstruction),
			secondInstruction(secondInstruction),
			id(id) {}
	} RaceEntry_;

	typedef std::vector< std::unique_ptr<RaceEntry_> > RaceEntries_;
	RaceEntries_ raceEntries_;

	void dumpRaceEntries(const char *fileName) const;

	// prevent generated functions --------------------------------------------
	RaceDetectionTool(const RaceDetectionTool&);
	RaceDetectionTool& operator=(const RaceDetectionTool&);
};



#endif /* RACEDETECTIONTOOL_H_ */
