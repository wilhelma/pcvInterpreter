set(TEST_INCLUDES ${pcv_SOURCE_DIR}/include/
                  ${pcv_SOURCE_DIR}/include/database/
                  ${pcv_SOURCE_DIR}/include/events/
                  ${pcv_SOURCE_DIR}/include/infos/
                  ${pcv_SOURCE_DIR}/include/shadows/
                  ${pcv_SOURCE_DIR}/include/tables/
                  ${pcv_SOURCE_DIR}/include/tools/)

set(TEST_SOURCES
    DatabaseReadTest.cxx
    EventGeneratorTest.cxx
    EventTestTool.cxx)

include_directories(${TEST_INCLUDES} ${GTEST_INC_PATH} ${EasyLoggingpp_INCLUDES})
add_executable(pcv_test EXCLUDE_FROM_ALL ${TEST_SOURCES})
target_link_libraries(pcv_test "sqlite3" ${GTEST_LIB} SQL_DB Interpreter -lpthread)
