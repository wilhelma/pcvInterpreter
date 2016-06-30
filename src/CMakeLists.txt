set(INCLUDE_DIRECTORIES ${SAAPFramework_SOURCE_DIR}/include
						${SAAPFramework_SOURCE_DIR}/include/events
						${SAAPFramework_SOURCE_DIR}/include/tables
						${SAAPFramework_SOURCE_DIR}/include/tools)
include_directories(${INCLUDE_DIRECTORIES})

set(SRC_LIST
	AccessInfo.cpp
	AcquireEvent.cpp
	CallEvent.cpp
	CallSiteHashtable.cpp
	CallSiteProfile.cpp
	DBInterpreter.cpp
	EventService.cpp
	Interpreter.cpp
	LockMgr.cpp
	main.cpp
	NewThreadEvent.cpp
	ParasiteStack.cpp
	ParasiteTool.cpp
	RaceDetectionTool.cpp
	ReleaseEvent.cpp

	SAAPRunner.cpp
	ThreadMgr.cpp)

set(SRC_LIST ${SRC_LIST}
	tools/FunctionTrackerTool.cpp
	tools/LockSetChecker.cpp
	tools/RaceDetectionTool.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)
