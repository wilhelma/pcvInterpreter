set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
include (GenerateExportHeader)

project(SAAPFramework)
cmake_minimum_required(VERSION 2.8)

## Check for Microsoft compiler
#if (MSVC)
#  # Include files
#  FILE(GLOB HDR_LIST *.h*)
#  source_group(include FILES ${HDR_LIST})
#
#  # Source files
#  FILE(GLOB SRC_LIST *.cpp)
#  source_group(source FILES ${SRC_LIST})
#else()
#	#  aux_source_directory(. SRC_LIST)
#endif (MSVC)

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

## Check if we're on a Unix-like operating system i.e. Linux, Apple OS X or CygWin
#if (UNIX)
#	#	set(SQLITE_INC_PATH ${SQLITE_PATH}/include)
#	#	set(SQLITE_LIB_PATH ${SQLITE_PATH}/lib)
#else ()
#	set(SQLITE_INCLUDE_DIR ${SQLITE_PATH})
#	set(SQLITE_LIBRARY_DIR ${SQLITE_PATH})
#endif (UNIX)
message(STATUS "SQLite Include Directory: ${SQLITE_INCLUDE_DIR}")
message(STATUS "SQLite Library Directory: ${SQLITE_LIBRARY_DIR}")

include_directories(${SQLITE_INCLUDE_DIR})
link_directories   (${SQLITE_LIBRARY_DIR})	

# ------------------------------------ CXX and C Flags --------------------------------------------- #

if (MSVC)
  set(CMAKE_CXX_FLAGS "/Gm- /errorReport:prompt /WX- /Zc:forScope /EHsc")
else ()
  set(CMAKE_CXX_FLAGS "-fPIC -DBIGARRAY_MULTIPLIER=1 -Wall -Wno-unknown-pragmas -std=c++11 -march=native")
  set(CMAKE_C_FLAGS   "-fPIC -DBIGARRAY_MULTIPLIER=1 -Wall -Wno-unknown-pragmas -march=native" )
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic ${CMAKE_SHARED_LINKER_FLAGS} ")
endif(MSVC)

# Add subdirectories
add_subdirectory(src)
