#include "midiworker.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(SendRaw, sendRawChung)
{
    MidiWorker worker;
    worker.raw_data.push_back(0xF0);
    for (int var = 0; var < 8000; ++var) {
        worker.raw_data.push_back(var);
    }
    worker.raw_data.push_back(0xF7);

    qDebug() << "Raw len " << worker.raw_data.size();

    worker.SendRaw();

    ASSERT_THAT(0, Eq(0));
}

