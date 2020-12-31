#include "oxihardware.h"

OxiHardware::OxiHardware(QObject *parent, QString deviceName) : QObject(parent)
{
    m_deviceDetected = false;
    m_deviceName = deviceName;
}

Q_INVOKABLE void OxiHardware::detectDevice() {
    bool ret = false;
    QMap<QString, QString> devices = QMidiOut::devices();
    for (QString device : devices.keys()) {
        QString  curDev = devices.value(device).toUtf8();
        if (curDev.compare(m_deviceName, Qt::CaseInsensitive) == 0) {
            ret = true;
            break;
        }
    }
    if (m_deviceDetected != ret) {
        m_deviceDetected = ret;
        emit connectionChanged(ret);
    }
}
