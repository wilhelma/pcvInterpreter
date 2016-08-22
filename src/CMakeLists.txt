set(INCLUDE_DIRECTORIES ${pcv_SOURCE_DIR}/include/
						${pcv_SOURCE_DIR}/include/database/
						${pcv_SOURCE_DIR}/include/events/
                        ${pcv_SOURCE_DIR}/include/hashtables/
						${pcv_SOURCE_DIR}/include/infos/
						${pcv_SOURCE_DIR}/include/profiles/
						${pcv_SOURCE_DIR}/include/shadows/
						${pcv_SOURCE_DIR}/include/tables/
						${pcv_SOURCE_DIR}/include/tools/)
include_directories(${INCLUDE_DIRECTORIES})

set(SRC_LIST
	DBInterpreter.cpp
	DAG.cpp
	EventService.cpp
	FunctionStack.cpp
	LockMgr.cpp
	main.cpp
	ParasiteJsonWriter.cpp
	ParasiteTracker.cpp
	ThreadMgr.cpp
	ThreadStack.cpp)

set(SRC_LIST ${SRC_LIST}
	database/DBManager.cpp)

set(SRC_LIST ${SRC_LIST}
	hashtables/CallSiteSpanHashtable.cpp
	hashtables/CallSiteWorkHashtable.cpp)

set(SRC_LIST ${SRC_LIST}
	profiles/CallSiteProfile.cpp
	profiles/CallSiteSpanProfile.cpp
	profiles/CallSiteWorkProfile.cpp)

set(SRC_LIST ${SRC_LIST}
	tables/Call.cpp)

set(SRC_LIST ${SRC_LIST}
	tools/DebugTool.cpp
	tools/FunctionTrackerTool.cpp
	tools/LockSetChecker.cpp
	tools/RaceDetectionTool.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)
