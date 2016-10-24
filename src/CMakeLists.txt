set(pcv_INCLUDES ${pcv_SOURCE_DIR}/include/
                 ${pcv_SOURCE_DIR}/include/database/
                 ${pcv_SOURCE_DIR}/include/events/
                 ${pcv_SOURCE_DIR}/include/hashtables/
                 ${pcv_SOURCE_DIR}/include/infos/
                 ${pcv_SOURCE_DIR}/include/profiles/
                 ${pcv_SOURCE_DIR}/include/shadows/
                 ${pcv_SOURCE_DIR}/include/tables/
                 ${pcv_SOURCE_DIR}/include/tools/
                 ${EasyLoggingpp_INCLUDES})

# Download rapidjson from its Github repository
set(RAPIDJSON_SOURCE_DIR ${pcv_SOURCE_DIR}/external/rapidjson)
ExternalProject_Add(rapidjson
    DOWNLOAD_DIR      ${RAPIDJSON_SOURCE_DIR}
    GIT_REPOSITORY    https://github.com/miloyip/rapidjson
    UPDATE_COMMAND    ${GIT_EXECUTABLE} pull origin master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    LOG_DOWNLOAD      ON)
set_target_properties(rapidjson PROPERTIES EXCLUDE_FROM_ALL TRUE)
ExternalProject_Get_Property(rapidjson source_dir)

# Add rapidjson's includes
set(pcv_INCLUDES ${pcv_INCLUDES} ${source_dir}/include)

include_directories(${pcv_INCLUDES})
set(SRC_LIST
    DAG.cpp
    DBInterpreter.cpp
    EventService.cpp
    FunctionStack.cpp
    LockIntervals.cpp
    LockMgr.cpp
    ParasiteJsonWriter.cpp
    ParasiteTracker.cpp
    SAAPRunner.cpp
    Span.cpp
    ThreadMgr.cpp
    ThreadStack.cpp
    Work.cpp
    main.cpp)

set(DB_SOURCES 
    tables/Call.cpp
    tables/File.cpp
    database/Database.cpp
    database/DBManager.cpp
    database/QueryResult.cpp)

# Add a rule to compile the database part in a shared library
add_library(SQL_DB SHARED EXCLUDE_FROM_ALL ${DB_SOURCES})
add_dependencies(SQL_DB easyloggingpp)
target_link_libraries(SQL_DB "sqlite3")

#set(SRC_LIST ${SRC_LIST} ${DB_SOURCES})

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
    tools/LockSetChecker.cpp
    tools/ParasiteTool.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
add_dependencies(${PROJECT_NAME} rapidjson easyloggingpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} -lpthread SQL_DB)
