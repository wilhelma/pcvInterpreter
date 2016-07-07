/*
 * RaceDetectionTool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef LOCK_SET_CHECKER_H_
#define LOCK_SET_CHECKER_H_

#include "Tool.h"

#include "fwd/AccessEvent.h"
#include "fwd/AcquireEvent.h"
#include "fwd/ReturnEvent.h"
#include "fwd/ReleaseEvent.h"
#include "fwd/NewThreadEvent.h"

#include "fwd/ShadowThread.h"
#include "fwd/ShadowVar.h"

#include "ShadowLock.h"

#include "Types.h"

#include <set>
#include <map>
#include <vector>
#include <memory>

#define THREADS 100

class LockSetChecker : public Tool {
public:
	LockSetChecker(const char* outFile);

	void Access(const AccessEvent* event) override final;
	void Acquire(const AcquireEvent* event) override final;
	void NewThread(const NewThreadEvent* event) override final;
	void Release(const ReleaseEvent* event) override final;

	~LockSetChecker();

	typedef enum { WRITE_READ = 1,		// read after write (RAW)
				   READ_WRITE = 2,		// write after read (WAR)
				   WRITE_WRITE = 3		// write after write (WAW)
	} RaceType;

//	static Decoration<ShadowThread, Set> locksheld;
	
private:

	virtual void Join(const JoinEvent* event) override final {};
	virtual void Call(const CallEvent* event) override final {};
	virtual void Return(const ReturnEvent* event) override final {};
	virtual void ThreadEnd(const ThreadEndEvent* event) override final {};

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

	
	typedef struct VarSet_ {
		LockSet_ lockset;
		INS_ID instruction;

		VarSet_() : instruction(0) {}
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
	LockSetChecker(const LockSetChecker&);
	LockSetChecker& operator=(const LockSetChecker&);
};



#endif /* LOCKSETCHECKER_H_ */
