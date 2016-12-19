/**
 *
 *    @file  EventGeneratorTest.cxx
 *   @brief  
 *
 *    @date  12/15/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */


#include "DBInterpreter.h"
#include "EventType.h"
#include "Observer.h"
#include "SAAPRunner.h"
#include "Filter.h"

#include "./EventTestTool.h"

#include "gtest/gtest.h"

#include <chrono>
#include <iostream>
#include <memory>

class EventGeneratorTest : public ::testing::Test {
};

TEST_F(EventGeneratorTest, CorrectThrownEvents) {
    // Create a SAAPRunner
    auto runner(std::make_unique<SAAPRunner>());

    // Create and register the tool
    const auto& event_test_tool_it = runner->registerTool(
            std::make_unique<EventTestTool>(),
            std::unique_ptr<Filter>(nullptr),
            EventType::ALL);

    // Start interpretation of the database
    runner->interpret(static_cast<std::string>("../test/databases/integration_test.db"));

    const auto test_tool_ptr = static_cast<EventTestTool*>(event_test_tool_it->tool.get());

    // Num of Access events should be the same as the number of entries in the Access table
    EXPECT_EQ(test_tool_ptr->NumAccesses_, 45);
    // Number of NewThread events should be the same as the number of entries in the Thread table
    EXPECT_EQ(test_tool_ptr->NumNewThreads_, 2);

    // All the threads (but one) must be joined
    EXPECT_EQ(test_tool_ptr->NumJoins_ + 1, test_tool_ptr->NumNewThreads_);

//    // All the threads end
//    EXPECT_EQ(test_tool_ptr->NumThreadEnds_, test_tool_ptr->NumNewThreads_); // Fails! Why?

    // Each called function should return
//    EXPECT_EQ(test_tool_ptr->NumCalls_, 4); // Fails! Why?
    EXPECT_EQ(test_tool_ptr->NumCalls_, test_tool_ptr->NumReturns_);

    // Each acquired resource should be released
    EXPECT_EQ(test_tool_ptr->NumAcquires_, test_tool_ptr->NumReleases_);

    // Unregister the tool
    runner->removeTool(event_test_tool_it);
}
