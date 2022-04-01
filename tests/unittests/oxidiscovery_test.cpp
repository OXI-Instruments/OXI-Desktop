#include "oxidiscovery.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(OxiDiscovery, oxiOutIndexTest)
{
    OxiDiscovery discovery = OxiDiscovery();
    int outIndex = discovery.GetOxiOutIndex(discovery.GetOutPorts());
    QString outDevName = discovery.GetOxiOutDeviceName(outIndex);
    qDebug() << "OUT Port Index: " << outIndex;
    qDebug() << "OUT Port Name: " << outDevName;
    ASSERT_NE(outIndex, -1);
}

TEST(OxiDiscovery, oxiInIndexTest)
{
    OxiDiscovery discovery = OxiDiscovery();
    int inIndex = discovery.GetOxiInIndex(discovery.GetInPorts());
    QString inDevName = discovery.GetOxiInDeviceName(inIndex);
    qDebug() << "IN Port Index: " << inIndex;
    qDebug() << "IN Port Name: " << inDevName;
    ASSERT_NE(inIndex, -1);
}
