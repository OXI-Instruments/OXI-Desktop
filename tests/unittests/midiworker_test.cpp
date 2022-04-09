#include "oxidiscovery.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <midiworker.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace testing;

TEST(Integration_Midiworker, enterFwUpdateTest)
{
    MidiWorker midiworker = MidiWorker(0, false);
    OxiDiscovery *discovery = midiworker.GetDiscovery();
    while(!discovery->IsConnected()){
        std::this_thread::sleep_for(1s);
    }
    midiworker.SendGotoBoot(MSG_FW_UPDT_OXI_ONE);

    // TODO: Implement handshake module that waits for ACK here instead of sleep
    std::this_thread::sleep_for(3s);

    bool inPortFwUpdate = discovery->IsInFwUpdate();
    bool outPortFwUpdate = discovery->IsOutFwUpdate();

    ASSERT_TRUE(inPortFwUpdate);
    ASSERT_TRUE(outPortFwUpdate);
}

TEST(Integration_Midiworker, exitFwUpdateTest)
{
    MidiWorker midiworker = MidiWorker(0, false);
    OxiDiscovery *discovery = midiworker.GetDiscovery();
    while(!discovery->IsConnected()){
        std::this_thread::sleep_for(1s);
    }
    midiworker.SendBootExit();

    // TODO: Implement handshake module that waits for ACK here instead of sleep
    std::this_thread::sleep_for(3s);

    bool inPortFwUpdate = discovery->IsInFwUpdate();
    bool outPortFwUpdate = discovery->IsOutFwUpdate();

    ASSERT_FALSE(inPortFwUpdate);
    ASSERT_FALSE(outPortFwUpdate);
}
