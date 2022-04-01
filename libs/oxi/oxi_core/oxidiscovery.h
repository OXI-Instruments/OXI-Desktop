#ifndef OXIDISCOVERY_H
#define OXIDISCOVERY_H

#include <QtWidgets>
#include "oxi_core_global.h"
#include "qmidiin.h"
#include "qmidiout.h"

class OXI_CORE_EXPORT OxiDiscovery : public QObject {
    Q_OBJECT
private:
    const static QString oxi;
    const static QString one;
    const static QString fw_update;

    QMidiIn* _midi_in;
    QMidiOut* _midi_out;
    int _in_idx;
    int _out_idx;

    bool IsOxiPort(QString portName);
    bool IsOxiPort(QStringList portNames, int index);
    bool IsOxiFwUpdatePort(QStringList portNames, int index);
    void DiscoverOutPort();
    void DiscoverInPort();

public:
    explicit OxiDiscovery(QMidiIn *midiIn, QMidiOut *midiOut);

    int GetOxiOutIndex(QStringList portNames);
    int GetOxiInIndex(QStringList portNames);
    QString GetOxiInDeviceName(int index);
    QString GetOxiOutDeviceName(int index);
    bool IsInFwUpdate();
    bool IsOutFwUpdate();
    QStringList GetOutPorts();
    QStringList GetInPorts();

signals:
    void ui_UpdateConnectionLabel(QString);

public slots:
    void Discover(void);
};

#endif // OXIDISCOVERY_H
