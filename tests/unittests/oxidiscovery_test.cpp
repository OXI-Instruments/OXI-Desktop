#include "oxidiscovery.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(OxiDiscovery, oxiOutIndexTest)
{
    QMidiIn midi_in;
    QMidiOut midi_out;
    OxiDiscovery discovery = OxiDiscovery(&midi_in, &midi_out);
    QStringList outPorts = midi_out.getPorts();
    int outIndex = discovery.GetOxiOutIndex(outPorts);
    QString outDevName = discovery.GetOxiOutDeviceName(outPorts, outIndex);
    qDebug() << "OUT Port Index: " << outIndex;
    qDebug() << "OUT Port Name: " << outDevName;
    ASSERT_NE(outIndex, -1);
}

TEST(OxiDiscovery, oxiInIndexTest)
{
    QMidiIn midi_in;
    QMidiOut midi_out;
    OxiDiscovery discovery = OxiDiscovery(&midi_in, &midi_out);
    QStringList inPorts = midi_in.getPorts();
    int inIndex = discovery.GetOxiInIndex(inPorts);
    QString inDevName = discovery.GetOxiInDeviceName(inPorts, inIndex);
    qDebug() << "IN Port Index: " << inIndex;
    qDebug() << "IN Port Name: " << inDevName;
    ASSERT_NE(inIndex, -1);
}
