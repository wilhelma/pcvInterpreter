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

# Common include directories
include_directories(${pcv_INCLUDES})

# > libSQL_DB.so --------------------------------------------------
find_path(SQLITE_PATH NAMES sqlite3.h PATHS $ENV{SQLITE_INC_PATH} DOC "SQLite's inc directory")

if(NOT SQLITE_PATH)
    message(FATAL "SQLite Include directory not found!\n"
        "Please set the environment variable SQLITE_INC_PATH to the base directory")
else()
    message(STATUS "SQLite Path: ${SQLITE_PATH}")
endif()

set(DB_SOURCES 
    database/Database.cpp
    database/DBManager.cpp
    database/QueryResult.cpp
    tables/Call.cpp
    tables/File.cpp)

# Add a rule to compile the database part in a shared library
add_library(SQL_DB SHARED EXCLUDE_FROM_ALL ${DB_SOURCES})
add_dependencies(SQL_DB easyloggingpp)
target_link_libraries(SQL_DB sqlite3)
# < libSQL_DB.so --------------------------------------------------

# > libInterpreter.a ----------------------------------------------
set(INTERPRETER_SOURCES
    DBInterpreter.cpp
    EventService.cpp
    LockMgr.cpp
    SAAPRunner.cpp
    ThreadMgr.cpp)

# Add a rule to compile the interpreter part in a static library
add_library(Interpreter STATIC EXCLUDE_FROM_ALL ${INTERPRETER_SOURCES})
add_dependencies(Interpreter easyloggingpp)
target_link_libraries(Interpreter SQL_DB)
# < libInterpreter.a ----------------------------------------------

# > libDebugTool.a ------------------------------------------------
set(DEBUG_TOOL_SOURCES tools/DebugTool.cpp)
add_library(DebugTool STATIC EXCLUDE_FROM_ALL ${DEBUG_TOOL_SOURCES})
add_dependencies(DebugTool easyloggingpp)
# Link DebugTool into main
set(LINKED_TOOLS ${LINKED_TOOLS} DebugTool)
# < libDebugTool.a ------------------------------------------------

# > libFunctionTrackerTool.a --------------------------------------
set(FUNCTION_TRACKER_SOURCES tools/FunctionTrackerTool.cpp)
add_library(FunctionTrackerTool STATIC EXCLUDE_FROM_ALL ${FUNCTION_TRACKER_SOURCES})
add_dependencies(FunctionTrackerTool easyloggingpp)
# Link FunctionTrackerTool into main
set(LINKED_TOOLS ${LINKED_TOOLS} FunctionTrackerTool)
# < libFunctionTrackerTool.a --------------------------------------

# > libLockSetChecker.a -------------------------------------------
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
include_directories(${source_dir}/include)

set(LOCK_SET_CHECKER_SOURCES tools/LockSetChecker.cpp)
add_library(LockSetChecker STATIC EXCLUDE_FROM_ALL ${LOCK_SET_CHECKER_SOURCES})
add_dependencies(LockSetChecker easyloggingpp rapidjson)
# Link LockSetChecker into main
set(LINKED_TOOLS ${LINKED_TOOLS} LockSetChecker)
# < libLockSetChecker.a -------------------------------------------

# > libParasiteTool.a ---------------------------------------------
find_path(BOOST_PATH NAMES boost/log/trivial.hpp PATHS $ENV{BOOST_PATH} DOC "Boost directory")
if(NOT BOOST_PATH)
    message(WARNING "BOOST directory not found!\n"
                    "Please set the environment variable BOOST_PATH to the base directory\n"
                    "Won't be able to compile ParasiteTool")
else(NOT BOOST_PATH)
    find_package(Boost COMPONENTS log log_setup thread date_time filesystem system REQUIRED)
    set(Boost_USE_STATIC_LIBS ON)

    include_directories(${Boost_INCLUDE_DIRS})
    link_directories   (${Boost_LIBRARY_DIR})


    set(PARASITE_SOURCES
        DAG.cpp
        FunctionStack.cpp
        LockIntervals.cpp
        ParasiteJsonWriter.cpp
        ParasiteTracker.cpp
        Span.cpp
        ThreadStack.cpp
        Work.cpp
        main.cpp)

    set(PARASITE_SOURCES ${PARASITE_SOURCES}
        profiles/CallSiteProfile.cpp
        profiles/CallSiteSpanProfile.cpp
        profiles/CallSiteWorkProfile.cpp)

    set(PARASITE_SOURCES ${PARASITE_SOURCES}
        tools/ParasiteTool.cpp)

    add_library(ParasiteTool STATIC EXCLUDE_FROM_ALL ${PARASITE_SOURCES})
    add_dependencies(ParasiteTool easyloggingpp rapidjson)

    # Link ParasiteTool into main
    set(LINKED_TOOLS ${LINKED_TOOLS} ParasiteTool)
    #${Boost_LIBRARIES} -lpthread 
endif(NOT BOOST_PATH)
# < libParasiteTool.a ---------------------------------------------

add_executable(${PROJECT_NAME} main.cpp)
add_dependencies(${PROJECT_NAME} easyloggingpp)
target_link_libraries(${PROJECT_NAME} Interpreter ${LINKED_TOOLS})
