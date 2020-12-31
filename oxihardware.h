#ifndef OXIHARDWARE_H
#define OXIHARDWARE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <qqml.h>

#include "QMidi/QMidiOut.h"
#include "QMidi/QMidiIn.h"

class OxiHardware : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    QML_ELEMENT
public:
    Q_INVOKABLE void detectDevice();
    OxiHardware(QObject *parent = nullptr, QString deviceName = nullptr);
    bool isConnected() const {return m_deviceDetected;};
    void setDeviceName(QString deviceName) {m_deviceName = deviceName;};

private:
    bool m_deviceDetected;
    QString m_deviceName;

signals:
    void connectionChanged(bool connected);

};

#endif // OXIHARDWARE_H
