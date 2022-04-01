#ifndef OXIDISCOVERY_H
#define OXIDISCOVERY_H

#include "midiworker.h"

class OXI_CORE_EXPORT OxiDiscovery
{
private:
    const static QString oxi;
    const static QString one;
    const static QString fw_update;

    MidiWorker *_midiWorker;
    int _in_idx;
    int _out_idx;

    bool IsOxiPort(QString portName);
    bool IsOxiPort(QStringList portNames, int index);
    void DiscoverOutPort(std::function<void (QString)> uiCallback);
    void DiscoverInPort();
public:
    OxiDiscovery(MidiWorker *midiWorker);

    void Discover(std::function<void (QString)> uiCallback);
    int GetOxiOutIndex(QStringList portNames);
    int GetOxiInIndex(QStringList portNames);
    QString GetOxiInDeviceName(int index);
    QString GetOxiOutDeviceName(int index);
    bool IsInFwUpdate();
    bool IsOutFwUpdate();
    QStringList GetOutPorts();
    QStringList GetInPorts();
};

#endif // OXIDISCOVERY_H
