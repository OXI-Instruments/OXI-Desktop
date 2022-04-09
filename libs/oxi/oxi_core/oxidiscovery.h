#ifndef OXIDISCOVERY_H
#define OXIDISCOVERY_H

#include <QtWidgets>
#include "oxi_core_global.h"
#include "qmidiin.h"
#include "qmidiout.h"

class OXI_CORE_EXPORT OxiDiscovery : public QThread {
    Q_OBJECT
private:
    const static QString oxi;
    const static QString one;
    const static QString fw_update;

    bool _stopPending;
    QMidiIn* _midi_in;
    QMidiOut* _midi_out;
    int _in_idx;
    int _out_idx;
    QString _oxi_port_name_in;
    QString _oxi_port_name_out;
    QStringList _previousInPorts;
    QStringList _previousOutPorts;

    bool IsOxiPort(QString portName);
    bool IsOxiPort(QStringList portNames, int index);
    bool IsOxiFwUpdatePort(QStringList portNames, int index);
    bool DiscoverOutPort();
    bool DiscoverInPort();

public:
    explicit OxiDiscovery(QMidiIn *midiIn, QMidiOut *midiOut, QObject *parent = nullptr);

    int GetOxiOutIndex(QStringList portNames);
    int GetOxiInIndex(QStringList portNames);
    QString GetOxiInDeviceName(int index);
    QString GetOxiOutDeviceName(int index);
    bool IsInFwUpdate();
    bool IsOutFwUpdate();
    QStringList GetOutPorts();
    QStringList GetInPorts();
    void run() override;
    void Stop();

    bool IsConnected();
signals:
    void ui_UpdateConnectionLabel(QString);
    void ui_PortAlreadyInUse(void);

public slots:
    void Discover(void);
};

#endif // OXIDISCOVERY_H
