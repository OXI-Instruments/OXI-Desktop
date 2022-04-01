#include "midiworker.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(dummy, simple_test_case)
{
    // Empty test for now to ensure it can run.
    MidiWorker midi_Worker = MidiWorker();
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

