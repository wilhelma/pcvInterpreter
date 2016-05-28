set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
include (GenerateExportHeader)

project(SAAPFramework)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)

if (MSVC)
  # Include files
  FILE(GLOB HDR_LIST *.h*)
  source_group(include FILES ${HDR_LIST})

  # Source files
  FILE(GLOB SRC_LIST *.cpp)
  source_group(source FILES ${SRC_LIST})
else()
  aux_source_directory(. SRC_LIST)
endif (MSVC)

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
message(STATUS ${Boost_INCLUDE_DIR})
message(STATUS ${Boost_LIBRARY_DIR})

include_directories(${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIR})

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

if (UNIX)
#	set(SQLITE_INC_PATH ${SQLITE_PATH}/include)
else ()
	set(SQLITE_INC_PATH ${SQLITE_PATH})
endif (UNIX)
message(STATUS "SQLITE_INC_PATH: ${SQLITE_INC_PATH}")

if (UNIX)
#	set(SQLITE_LIB_PATH ${SQLITE_PATH}/lib)
else ()
	set(SQLITE_LIB_PATH ${SQLITE_PATH})
endif (UNIX)
message(STATUS "SQLITE_LIB_PATH: ${SQLITE_LIB_PATH}")

include_directories(${SQLITE_INC_PATH})
link_directories (${SQLITE_LIB_PATH})	

# ------------------------------------ CXX and C Flags --------------------------------------------- #

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE}/)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/Gm- /errorReport:prompt /WX- /Zc:forScope /EHsc")
else ()
  set(CMAKE_CXX_FLAGS "-fPIC -DBIGARRAY_MULTIPLIER=1 -Wall -Wno-unknown-pragmas -std=c++11")
  set(CMAKE_C_FLAGS "-fPIC -DBIGARRAY_MULTIPLIER=1 -WALL -Wno-unknown-pragmas -std=c++11" )
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic ${CMAKE_SHARED_LINKER_FLAGS} ")
endif(MSVC)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HDR_LIST})

target_link_libraries(${PROJECT_NAME} "sqlite3" ${Boost_LIBRARIES} -lpthread)

