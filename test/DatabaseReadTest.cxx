/**
 *
 *    @file  DatabaseReadTest.cxx
 *   @brief  Test to check that databases are properly read.
 *
 *    @date  10/18/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "Database.h"
#include "gtest/gtest.h"

// Database tables
#include "AccessTable.h"
#include "CallTable.h"
#include "FileTable.h"
#include "FunctionTable.h"
#include "InstructionTable.h"
#include "LoopTable.h"
#include "LoopExecutionTable.h"
#include "LoopIterationTable.h"
#include "ReferenceTable.h"
#include "SegmentTable.h"
#include "ThreadTable.h"
#include "DBTable.h"

#include "Access.h"
#include "Call.h"
#include "File.h"
#include "Function.h"
#include "Instruction.h"
#include "Loop.h"
#include "LoopExecution.h"
#include "LoopIteration.h"
#include "Reference.h"
#include "Segment.h"
#include "Thread.h"

#include "easylogging++.h"

#include <chrono>
#include <iostream>
#include <memory>

class DatabaseReadTest : public ::testing::Test {
};

TEST_F(DatabaseReadTest, DatabaseProperlyRead) {

    auto start_db = std::chrono::system_clock::now();
    
    std::unique_ptr<const Database> db;
    // Throws if database can't be closed
    EXPECT_NO_THROW(db = load_database("../test/databases/integration_test.db"));

    auto elapsed_db = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_db);
    std::cout << std::endl
              << "Database loaded in: " << elapsed_db.count() << " ms"
              << std::endl;

    // Test whether the SQLIterator runs through the whole database
    ASSERT_EQ(db->accessTable()->size(),       45);
    ASSERT_EQ(db->callTable()->size(),          4);
    ASSERT_EQ(db->fileTable()->size(),          2);
    ASSERT_EQ(db->functionTable()->size(),     13);
//  ASSERT_EQ(db->imageTable()->size(),         1); // TODO
    ASSERT_EQ(db->instructionTable()->size(),  51);
    ASSERT_EQ(db->loopTable()->size(),          0);
    ASSERT_EQ(db->loopExecutionTable()->size(), 0);
    ASSERT_EQ(db->loopIterationTable()->size(), 0);
//  ASSERT_EQ(db->memberTable()->size(),        0); // TODO
    ASSERT_EQ(db->referenceTable()->size(),    12);
    ASSERT_EQ(db->segmentTable()->size(),       6);
    ASSERT_EQ(db->threadTable()->size(),        2);
}

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
