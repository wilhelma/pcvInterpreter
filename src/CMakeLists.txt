set(INCLUDE_DIRECTORIES ${pcv_SOURCE_DIR}/include/
                        ${pcv_SOURCE_DIR}/include/database/
                        ${pcv_SOURCE_DIR}/include/events/
                        ${pcv_SOURCE_DIR}/include/hashtables/
                        ${pcv_SOURCE_DIR}/include/infos/
                        ${pcv_SOURCE_DIR}/include/profiles/
                        ${pcv_SOURCE_DIR}/include/shadows/
                        ${pcv_SOURCE_DIR}/include/tables/
                        ${pcv_SOURCE_DIR}/include/tools/)

# Download rapidjson from its Github repository
include(ExternalProject)
set(RAPIDJSON_SOURCE_DIR ${pcv_SOURCE_DIR}/external/rapidjson)
ExternalProject_Add(
    rapidjson
    DOWNLOAD_DIR      ${RAPIDJSON_SOURCE_DIR}
    GIT_REPOSITORY    https://github.com/miloyip/rapidjson
    UPDATE_COMMAND    ${GIT_EXECUTABLE} pull origin master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    LOG_DOWNLOAD      ON)
set_target_properties(rapidjson PROPERTIES EXCLUDE_FROM_ALL TRUE)
ExternalProject_Get_Property(rapidjson source_dir)

include_directories(${INCLUDE_DIRECTORIES})
# Add rapidjson's includes
include_directories(${INCLUDE_DIRECTORIES} ${source_dir}/include)

set(SRC_LIST
    DBInterpreter.cpp
    EventService.cpp
    FunctionStack.cpp
    LockIntervals.cpp
    LockMgr.cpp
    main.cpp
    ParasiteJsonWriter.cpp
    ParasiteTracker.cpp
    ThreadMgr.cpp
    ThreadStack.cpp
    Span.cpp
    Work.cpp)

set(SRC_LIST ${SRC_LIST}
    database/DBManager.cpp
    database/QueryResult.cpp)

set(SRC_LIST ${SRC_LIST}
    profiles/CallSiteProfile.cpp
    profiles/CallSiteSpanProfile.cpp
    profiles/CallSiteWorkProfile.cpp)

set(SRC_LIST ${SRC_LIST}
    tables/Call.cpp
    tables/File.cpp)

set(SRC_LIST ${SRC_LIST}
    tools/DebugTool.cpp
    tools/FunctionTrackerTool.cpp
    # tools/LockSetChecker.cpp
    tools/SequenceTool.cpp
    tools/ParasiteTool.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
add_dependencies(${PROJECT_NAME} rapidjson)
target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)
