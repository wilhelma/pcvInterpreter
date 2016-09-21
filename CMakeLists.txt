project(pcv)
cmake_minimum_required(VERSION 2.8)

FILE(GLOB_RECURSE LibFiles "*.h")
add_custom_target(headers SOURCES ${LibFiles})

# Add a custom 'doc' target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

# ---------------------------------- BOOST related search map ---------------------------------------- #
find_path(BOOST_PATH
  NAMES boost/log/trivial.hpp
  PATHS $ENV{BOOST_PATH}
  DOC "Boost directory"
)

if(NOT BOOST_PATH)
  message(FATAL_ERROR
    "\nBOOST directory not found!\n"
    "Please set the environment variable BOOST_PATH to the base directory"
  )
endif(NOT BOOST_PATH)

#add_definitions(-DBOOST_LOG_DYN_LINK)
set(Boost_USE_STATIC_LIBS ON)
# thread
find_package(Boost COMPONENTS log log_setup thread date_time filesystem system REQUIRED)
message(STATUS "Boost  Include Directory: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost  Library Directory: ${Boost_LIBRARY_DIR}")

include_directories(${Boost_INCLUDE_DIRS})
link_directories   (${Boost_LIBRARY_DIR})

# --------------------------------- SQLITE related search map ---------------------------------------- #

find_path(SQLITE_PATH
  NAMES sqlite3.h
  PATHS $ENV{SQLITE_INC_PATH}
  DOC "SQLite's inc directory"
)

if(NOT SQLITE_PATH)
  message(FATAL_ERROR
    "\nSQLite Include directory not found!\n"
    "Please set the environment variable SQLITE_INC_PATH to the base directory"
  )
endif(NOT SQLITE_PATH)

message(STATUS "SQLite Include Directory: ${SQLITE_INCLUDE_DIR}")
message(STATUS "SQLite Library Directory: ${SQLITE_LIBRARY_DIR}")

include_directories(${SQLITE_INCLUDE_DIR})
link_directories   (${SQLITE_LIBRARY_DIR})  

# ------------------------------------ CXX and C Flags --------------------------------------------- #

set(CMAKE_CXX_FLAGS "-fPIC -DBIGARRAY_MULTIPLIER=1 -pedantic -g -Wall -Wno-unknown-pragmas -std=c++14 -march=native")
#set(CMAKE_C_FLAGS   "-fPIC -DBIGARRAY_MULTIPLIER=1 -g -Wall -Wno-unknown-pragmas -march=native" )
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic ${CMAKE_SHARED_LINKER_FLAGS} ")

# Add subdirectories
find_package(Git)
if (GIT_FOUND)
    add_subdirectory(src)
else (GIT_FOUND)
    message(FATAL "Git not found: cannot download rapidjson. Please install git.")
endif (GIT_FOUND)
