/*
 * RaceDetectionTool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef LOCKSETCHECKER_H_
#define LOCKSETCHECKER_H_

#include "Tool.h"

#include <iostream>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include "Interpreter.h"
#include "Event.h"
#include "AccessEvent.h"
#include "AcquireEvent.h"
#include "NewThreadEvent.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"

#include "Types.h"

#define THREADS 100

class LockSetChecker : public Tool {
public:
	LockSetChecker(const char* outFile);
	void create(const Event* e) override;
	void join(const Event* e) override;
	void acquire(const Event* e) override;
	void release(const Event* e) override;
	void access(const Event* e) override;
	void call(const Event* e) override;
	~LockSetChecker();

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
