project(pcv)
cmake_minimum_required(VERSION 2.8)

# Add a custom 'doc' target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    file(MAKE_DIRECTORY ${pcv_BINARY_DIR}/doc/html)
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DBIGARRAY_MULTIPLIER=1 -pedantic -g -Wall -Wno-unknown-pragmas -std=c++14 -march=native")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic ${CMAKE_SHARED_LINKER_FLAGS} ")

# Add subdirectories
find_package(Git)
if (GIT_FOUND)

    # Download EasyLogging++ from its Github repository
    include(ExternalProject)
    set(EXTERNAL_DIR ${pcv_BINARY_DIR}/external/)
    ExternalProject_Add(easyloggingpp
        PREFIX            ${EXTERNAL}
        GIT_REPOSITORY    https://github.com/easylogging/easyloggingpp
        GIT_TAG           9.84
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        LOG_DOWNLOAD      ON)
    set_target_properties(easyloggingpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
    ExternalProject_Get_Property(easyloggingpp source_dir)

    # Add EasyLogging++'s includes
    set(EasyLoggingpp_INCLUDES ${source_dir}/src)

    add_subdirectory(src)
else (GIT_FOUND)
    message(FATAL "Git not found: cannot download rapidjson nor EasyLogging++. Please install git.")
endif (GIT_FOUND)


# Check for GTest libs and includes
find_library(GTEST_LIB gtest)
find_path(GTEST_INC_PATH gtest/gtest.h)

if (GTEST_LIB AND GTEST_INC_PATH)
    # Add test target
    enable_testing()
    add_subdirectory(test)
    add_test(NAME DatabaseTest COMMAND pcv_test)
else ()
    message(WARNING "GTest not found: please install it if you want to run the tests.")
endif ()
