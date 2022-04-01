#include "oxidiscovery.h"
#include <QStringList>

const QString OxiDiscovery::oxi = QString("OXI");
const QString OxiDiscovery::one = QString("ONE");
const QString OxiDiscovery::fw_update = QString("FW Update");

OxiDiscovery::OxiDiscovery()
{
    _in_idx = -1;
    _out_idx = -1;
}

QStringList OxiDiscovery::GetOutPorts(){
    return _midi_out.getPorts();
}

QStringList OxiDiscovery::GetInPorts(){
    return _midi_in.getPorts();
}

bool OxiDiscovery::IsOxiFwUpdatePort(QStringList portNames, int index){
    return IsOxiPort(portNames, index)
            && portNames[index].contains(fw_update);
}

bool OxiDiscovery::IsOxiPort(QStringList portNames, int index){
    return index >= 0
            && index < portNames.length()
            && IsOxiPort(portNames[index]);
}

bool OxiDiscovery::IsOxiPort(QString portName){
    bool isOxiPortName = (portName.contains(one) || (portName.contains(fw_update))) &&
            portName.contains(oxi);
    return isOxiPortName;
}

int OxiDiscovery::GetOxiOutIndex(QStringList portNames)
{
    qDebug() << "OUT ports are: " << portNames;
    for (int i = 0; i != portNames.size(); ++i)
    {
        if (IsOxiPort(portNames[i]))
        {
            return i;
        }
    }

    return -1;
}

int OxiDiscovery::GetOxiInIndex(QStringList portNames)
{
    qDebug() << "IN ports are: " << portNames;
    for (int i = 0; i != portNames.size(); ++i)
    {
        if (IsOxiPort(portNames[i]))
        {
            return i;
        }
    }

    return -1;
}

QString OxiDiscovery::GetOxiOutDeviceName(int index){
    return _midi_out.getPorts()[index];
}

QString OxiDiscovery::GetOxiInDeviceName(int index){
    return _midi_in.getPorts()[index];
}

bool OxiDiscovery::IsInFwUpdate(){
    QStringList inPorts = _midi_in.getPorts();
    return IsOxiFwUpdatePort(inPorts, _in_idx);
}

bool OxiDiscovery::IsOutFwUpdate(){
    QStringList inPorts = _midi_in.getPorts();
    return IsOxiFwUpdatePort(inPorts, _in_idx);
}

void OxiDiscovery::Discover(std::function<void(QString)> uiCallback)
{
    DiscoverOutPort(uiCallback);
    DiscoverInPort();
}

void OxiDiscovery::DiscoverOutPort(std::function<void(QString)> uiCallback){
    QStringList outPorts = _midi_out.getPorts();
    if (_out_idx >= 0)
    {
        bool isOxiPort = IsOxiPort(outPorts, _out_idx);
        bool isOpen = _midi_out.isPortOpen();

        if (!isOxiPort || !isOpen){
            qDebug() << "MIDI OUT port index: " << _out_idx;
            qDebug() << "MIDI OUT ports: " << outPorts;
            qWarning() << "MIDI OUT port is not an OXI port and will be closed";
            _out_idx = -1;
            _midi_out.closePort();
            qInfo() << "The port has been closed";
        }
    }
    else {
        uiCallback("CONNECT YOUR OXI ONE");
        int oxiOutIdx = GetOxiOutIndex(outPorts);
        if (oxiOutIdx >= 0){
            qInfo() << "Discovered MIDI OUT port " + outPorts[oxiOutIdx] + " and will open";
            _midi_out.openPort(oxiOutIdx);
            qInfo() << "Port opened to " + outPorts[oxiOutIdx];
            _out_idx = oxiOutIdx;
            uiCallback("OXI ONE CONNECTED");
        }
    }
}

void OxiDiscovery::DiscoverInPort(){
    QStringList inPorts = _midi_in.getPorts();
    if (_in_idx >= 0)
    {
        bool isOxiPort = IsOxiPort(inPorts, _in_idx);
        bool isOpen = _midi_in.isPortOpen();

        if (!isOxiPort || !isOpen){
            qDebug() << "MIDI IN port index: " << _in_idx;
            qDebug() << "MIDI IN ports: " << inPorts;
            qWarning() << "MIDI IN port is not an OXI port and will be closed";
            _in_idx = -1;
            _midi_in.closePort();
            qInfo() << "The port has been closed";
        }
    }
    else {
        int oxiInIdx = GetOxiInIndex(inPorts);
        if (oxiInIdx >= 0){
            qInfo() << "Discovered MIDI IN port" + inPorts[oxiInIdx] + " and will open";
            _midi_in.openPort(oxiInIdx);
            qInfo() << "Port opened to " + inPorts[oxiInIdx];
            _in_idx = oxiInIdx;
        }
    }
}
