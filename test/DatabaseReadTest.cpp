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

std::string db_file_name = "../test/databases/integration_test.db";

TEST_F(DatabaseReadTest, DatabaseProperlyRead) {

    auto start_db = std::chrono::system_clock::now();
    
    std::unique_ptr<const Database> db;
    // Throws if database can't be closed
    EXPECT_NO_THROW(db = load_database(db_file_name));

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

    // Check correctness of read data.
    ACC_ID acc_id{15};
    const auto& a = access_with_id(acc_id, *db);
    EXPECT_EQ(a.instruction_id, 18);
    EXPECT_EQ(a.position,        0);
    EXPECT_EQ(a.reference_id,    4);
    EXPECT_EQ(a.access_type, AccessType::READ);
    EXPECT_EQ(a.memory_state, AccessState::INIT);

    CAL_ID cal_id{4};
    const auto& c = call_with_id(cal_id, *db);
    EXPECT_EQ(c.thread_id,      1);
    EXPECT_EQ(c.function_id,    9);
    EXPECT_EQ(c.instruction_id, 0); // NULL in SQL is read as 0.
    EXPECT_EQ(c.start_time, 40928559884969);
    EXPECT_EQ(c.end_time,   40928570173223);

    FIL_ID fil_id{1};
    const auto fi = db->fileTable()->find(fil_id);
    ASSERT_NE(fi, std::cend(*db->fileTable()));
    EXPECT_EQ(fi->second.file_path, "/home/wilhelma/workspace/tests/parceive/pcvInterpreter/integration_test.c");
    EXPECT_EQ(fi->second.file_name, "integration_test.c");

    FUN_ID fun_id{11};
    const auto fu = db->functionTable()->find(fun_id);
    ASSERT_NE(fu, std::cend(*db->functionTable()));
    EXPECT_EQ(fu->second.name,      "main");
    EXPECT_EQ(fu->second.signature, "main");
    EXPECT_EQ(fu->second.file_id,     0);
    EXPECT_EQ(fu->second.line_number, 0);
//    EXPECT_EQ(fu->second.column,      0); // to be implemented
    EXPECT_EQ(fu->second.type, FunctionType::METHOD);

    INS_ID ins_id{21};
    const auto& i = db->instructionTable()->find(ins_id);
    ASSERT_NE(i, std::cend(*db->instructionTable()));
    EXPECT_EQ(i->second.segment_id,   5);
    EXPECT_EQ(i->second.instruction_type, InstructionType::ALLOC);
    EXPECT_EQ(i->second.line_number, 43);
//    EXPECT_EQ(i->second.column,      0); // to be implemented

    // Loops are not there.

    REF_ID ref_id{8};
    const auto& r = db->referenceTable()->find(ref_id);
    ASSERT_NE(r, std::cend(*db->referenceTable()));
    EXPECT_EQ(r->second.size,         8);
    EXPECT_EQ(r->second.memory_type, ReferenceType::STACK);
    EXPECT_EQ(r->second.name, "thread");
    EXPECT_EQ(r->second.allocinstr,  11);

    SEG_ID seg_id{4};
    const auto& s = db->segmentTable()->find(seg_id);
    ASSERT_NE(s, std::cend(*db->segmentTable()));
    EXPECT_EQ(s->second.call_id,      3);
    EXPECT_EQ(s->second.segment_type, SegmentType::REGION);
    EXPECT_EQ(s->second.loop_pointer, 0);

    TRD_ID trd_id{1};
    const auto& t = thread_with_id(trd_id, *db);
    EXPECT_EQ(t.start_time, "2016-10-20T11:56:42.157583550Z");
    EXPECT_EQ(t.end_time,   "2016-10-20T11:56:42.170146129Z");
    EXPECT_EQ(t.start_cycle, 40928550606636);
    EXPECT_EQ(t.num_cycles,        32679275);
    EXPECT_EQ(t.create_instruction_id,   25);
    EXPECT_EQ(t.join_instruction_id,     49);
    EXPECT_EQ(t.parent_thread_id,         0);
    EXPECT_EQ(t.process_id,           18120);
    EXPECT_EQ(t.call_id,                  4);
}

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
    if (argc == 2)
        db_file_name = std::string(argv[1]);

    // Dirty trick to suppress output (on Linux)
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "\r\x1b[A");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
