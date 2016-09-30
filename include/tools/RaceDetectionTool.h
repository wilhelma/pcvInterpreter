/*
 * RaceDetectionTool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef RACEDETECTIONTOOL_H_
#define RACEDETECTIONTOOL_H_

#include "Tool.h"

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/JoinEvent.h"
#include "fwd/NewThreadEvent.h"

#include "fwd/ShadowThread.h"
#include "fwd/ShadowVar.h"

#include "ShadowLock.h"
#include "Types.h"

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <memory>

#define THREADS 100

/// @ingroup observers
class RaceDetectionTool : public Tool {
public:
	RaceDetectionTool(const char* outFile);
	~RaceDetectionTool();

	virtual void NewThread(const NewThreadEvent* e) override final;
	virtual void Join(const JoinEvent* e) override final;
	virtual void Acquire(const AcquireEvent* e) override final;
	virtual void Release(const ReleaseEvent* e) override final;
	virtual void Access(const AccessEvent* e) override final;

	typedef enum { WRITE_READ = 1,		// read after write (RAW)
				   READ_WRITE = 2,		// write after read (WAR)
				   WRITE_WRITE = 3		// write after write (WAW)
	} RaceType;

//	static Decoration<ShadowThread, Set> locksheld;
	
private:
	virtual void Call(const CallEvent* e) override final {};
	virtual void ThreadEnd(const ThreadEndEvent* e) override final {};
	virtual void Return(const ReturnEvent* e) override final {};

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
	typedef std::map<TRD_ID, VectorClock_> ThreadVC_;
	typedef struct Epoch_ {
		TRD_ID threadId;
		Clock_ clock;
		Epoch_(TRD_ID threadId, Clock_ clock) 
			: threadId(threadId), clock(clock) {}
		void set(TRD_ID tId, Clock_ clck) {
			threadId = tId;
			clock = clck;
		}
		bool operator ==(const struct Epoch_ &e2) const {
			return ( threadId == e2.threadId && clock == e2.clock );
		}
	} Epoch_;

	ThreadVC_ threadVC_;

	inline void vcMerge(VectorClock_& lhs, const VectorClock_& rhs) const;
	inline bool vcLEQ(const Epoch_& epoch, const VectorClock_& vc) const;

	
	typedef struct VarSet_ {
		Epoch_ epoch;
		LockSet_ lockset;
		INS_ID instruction;

		VarSet_() : epoch((TRD_ID)0,(Clock_)0), instruction(0) {}
	} VarSet_;

	typedef std::map<TRD_ID, VarSet_> ThreadVarSet_;
	typedef std::map<REF_ID, ThreadVarSet_> ReadVarSet_;
	typedef std::map<REF_ID, VarSet_> WriteVarSet_;
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
