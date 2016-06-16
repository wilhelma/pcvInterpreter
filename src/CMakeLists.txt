set(INCLUDE_DIRECTORIES ${SAAPFramework_SOURCE_DIR}/include
						${SAAPFramework_SOURCE_DIR}/include/tables
						${SAAPFramework_SOURCE_DIR}/include/tools)
include_directories(${INCLUDE_DIRECTORIES})
set(SRC_LIST
	AccessInfo.cpp
	AcquireEvent.cpp
	CallEvent.cpp
	DBInterpreter.cpp
	dotFuncTracker.cpp
	Event.cpp
	EventService.cpp
	FunctionTrackerTool.cpp
	Interpreter.cpp
	JoinEvent.cpp
	LockMgr.cpp
	LockSetChecker.cpp
	main.cpp
	NewThreadEvent.cpp
	RaceDetectionTool.cpp
	ReleaseEvent.cpp
	SAAPRunner.cpp
	ShadowLock.cpp
	ShadowThread.cpp
	ThreadMgr.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)
