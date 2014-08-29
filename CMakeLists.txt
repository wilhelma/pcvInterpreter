include (GenerateExportHeader)

project(SAAPFramework)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)
aux_source_directory(. SRC_LIST)

# --------------------------------- SQLITE related seach map ---------------------------------------- #

find_path(SQLITE_PATH
  NAMES include/sqlite3.h
  PATHS $ENV{SQLITE_PATH}
  DOC "SQLite's inc directory"
)

if(NOT SQLITE_PATH)
  message(FATAL_ERROR
    "\nSQLite Include directory not found!\n"
    "Please set the environment variable SQLITE_INC_PATH to the base directory"
  )
endif(NOT SQLITE_PATH)
set(SQLITE_INC_PATH ${SQLITE_PATH}/include)
message(STATUS "SQLITE_INC_PATH: ${SQLITE_INC_PATH}")

set(SQLITE_LIB_PATH ${SQLITE_PATH}/lib)
message(STATUS "SQLITE_LIB_PATH: ${SQLITE_LIB_PATH}")


include_directories(${SQLITE_INC_PATH})
link_directories (${SQLITE_LIB_PATH})	

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE}/)

if(UNIX)
SET(CMAKE_CXX_FLAGS "-fPIC")
SET(CMAKE_C_FLAGS "-fPIC")
endif(UNIX)

if(UNIX)
	set(CMAKE_CXX_FLAGS "-DBIGARRAY_MULTIPLIER=1 -Wall -Wno-unknown-pragmas -std=c++11")
	set(CMAKE_C_FLAGS "-DBIGARRAY_MULTIPLIER=1 -WALL -Wno-unknown-pragmas -std=c++11 ")
	set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic ${CMAKE_SHARED_LINKER_FLAGS} ")
#	add_definitions(-DBIGARRAY_MULTIPLIER=1 -DTARGET_IA32E -DHOST_IA32E -DTARGET_LINUX)
endif(UNIX)

add_executable(${PROJECT_NAME} ${SRC_LIST})

if(UNIX)
	target_link_libraries(${PROJECT_NAME} "sqlite3")

endif(UNIX)

