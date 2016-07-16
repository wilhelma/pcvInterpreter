set(INCLUDE_DIRECTORIES ${pcv_SOURCE_DIR}/include/
						${pcv_SOURCE_DIR}/include/events/
						${pcv_SOURCE_DIR}/include/infos/
						${pcv_SOURCE_DIR}/include/tables/
						${pcv_SOURCE_DIR}/include/tools/)
include_directories(${INCLUDE_DIRECTORIES})

set(SRC_LIST ${SRC_LIST}
	CallSiteHashtable.cpp
	CallSiteProfile.cpp
	CallSiteEndProfile.cpp
	DBInterpreter.cpp
	EventService.cpp
	FunctionStack.cpp
	Interpreter.cpp
	LockMgr.cpp
	main.cpp
	ParasiteTracker.cpp
	SAAPRunner.cpp
	ThreadMgr.cpp
	ThreadStack.cpp)

set(SRC_LIST ${SRC_LIST}
	tools/DebugTool.cpp
	tools/FunctionTrackerTool.cpp
        tools/LockSetChecker.cpp)
#	tools/ParasiteTool.cpp)

set(SRC_LIST ${SRC_LIST}
	tables/AccessTable.cpp
	tables/Call.cpp
	tables/CallTable.cpp
	tables/FileTable.cpp
	tables/FunctionTable.cpp
	tables/InstructionTable.cpp
	tables/LoopExecutionTable.cpp
	tables/LoopIterationTable.cpp
	tables/LoopTable.cpp
	tables/ReferenceTable.cpp
	tables/SegmentTable.cpp
	tables/ThreadTable.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)
