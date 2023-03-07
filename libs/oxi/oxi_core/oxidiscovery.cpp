#include "oxidiscovery.h"
#include <QStringList>

const QString OxiDiscovery::oxi = QString("OXI");
const QString OxiDiscovery::one = QString("ONE");
const QString OxiDiscovery::fw_update = QString("FW Update");

OxiDiscovery::OxiDiscovery(QMidiIn *midiIn, QMidiOut *midiOut)
{
    _in_idx = -1;
    _out_idx = -1;
    _midi_in = midiIn;
    _midi_out = midiOut;
    _previousInPorts = QStringList();
    _previousOutPorts = QStringList();
}

QStringList OxiDiscovery::GetOutPorts(){
    return _midi_out->getPorts();
}

QStringList OxiDiscovery::GetInPorts(){
    return _midi_in->getPorts();
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
    QStringList ports = _midi_out->getPorts();
    return ports[index];
}

QString OxiDiscovery::GetOxiInDeviceName(int index){
    QStringList ports = _midi_out->getPorts();
    return ports[index];
}

bool OxiDiscovery::IsInFwUpdate(){
    QStringList inPorts = _midi_in->getPorts();
    return IsOxiFwUpdatePort(inPorts, _in_idx);
}

bool OxiDiscovery::IsOutFwUpdate(){
    QStringList inPorts = _midi_in->getPorts();
    return IsOxiFwUpdatePort(inPorts, _in_idx);
}

void OxiDiscovery::Discover()
{
    if(DiscoverOutPort() || DiscoverInPort()){
        emit ui_PortAlreadyInUse();
    }

    if (_out_idx != -1 &&
            _in_idx != -1) {
        if (!connected) {
            connected = true;
            if (IsInFwUpdate()) {
                // Nothing
            } else {
                // SetFwVersion
                emit discoveryOxiConnected();
            }
        }
    } else {
        connected = false;
    }

}

bool OxiDiscovery::DiscoverOutPort(){
    QStringList outPorts = _midi_out->getPorts();
    if(outPorts != _previousOutPorts){
        qInfo() << "MIDI OUT ports have changed: " << outPorts;
        _previousOutPorts = outPorts;
    }

    // this checks if it's an oxi one
    int oxiOutIdx = GetOxiOutIndex(outPorts);

    if (_out_idx >= 0)
    {
        bool isOxiPort = IsOxiPort(outPorts, _out_idx);
        bool isOpen = _midi_out->isPortOpen();

        QString oxi_port_name_current = "";
        if (oxiOutIdx >= 0) {
            oxi_port_name_current = outPorts[oxiOutIdx];
        }

        if (!isOxiPort || !isOpen || _oxi_port_name_out != oxi_port_name_current){
            qDebug() << "MIDI OUT port index: " << _out_idx;
            qDebug() << "MIDI OUT ports: " << outPorts;
            qWarning() << "MIDI OUT port is not an OXI port and will be closed";
            _out_idx = -1;
            _midi_out->closePort();
            _oxi_port_name_out = "";
            qInfo() << "The port has been closed";
        }
    }
    else {
        emit ui_UpdateConnectionLabel("CONNECT YOUR OXI ONE");
//        int oxiOutIdx = GetOxiOutIndex(outPorts);
        if (oxiOutIdx >= 0){
            qInfo() << "Discovered MIDI OUT port " + outPorts[oxiOutIdx] + " and will open";
            try{
                _midi_out->openPort(oxiOutIdx);
            }
            catch ( RtMidiError &error ) {
                qWarning() << &error.getMessage();
                return true;
            }
            _oxi_port_name_out = outPorts[oxiOutIdx];
            qInfo() << "Port opened to " + _oxi_port_name_in;
            _out_idx = oxiOutIdx;

            emit ui_UpdateConnectionLabel("OXI ONE CONNECTED");
        }
    }

    return false;
}

bool OxiDiscovery::DiscoverInPort(){
    QStringList inPorts = _midi_in->getPorts();
    if(inPorts != _previousInPorts){
        qInfo() << "MIDI IN ports have changed: " << inPorts;
        _previousInPorts = inPorts;
    }

    int oxiInIdx = GetOxiInIndex(inPorts);

    if (_in_idx >= 0)
    {
        bool isOxiPort = IsOxiPort(inPorts, _in_idx);
        bool isOpen = _midi_in->isPortOpen();
        QString oxi_port_name_current = "";
        if (oxiInIdx >= 0) {
            oxi_port_name_current = inPorts[oxiInIdx];
        }

        if (!isOxiPort || !isOpen || _oxi_port_name_in != oxi_port_name_current){
            qDebug() << "MIDI IN port index: " << _in_idx;
            qDebug() << "MIDI IN ports: " << inPorts;
            qWarning() << "MIDI IN port is not an OXI port and will be closed";
            _in_idx = -1;
            _oxi_port_name_in = "";
            _midi_in->closePort();
            qInfo() << "The port has been closed";
        }
    }
    else {
//        int oxiInIdx = GetOxiInIndex(inPorts);
        if (oxiInIdx >= 0){
            qInfo() << "Discovered MIDI IN port " + inPorts[oxiInIdx] + " and will open";
            try{
                // Messages across multiple buffers currently not implemented.
                // https://www.music.mcgill.ca/~gary/rtmidi/classRtMidiIn.html#a7bf07fe12fa6588db8e772f0fc56f70d
                _midi_in->setBufferSize(20000, 1);
                _midi_in->openPort(oxiInIdx);
            }
            catch ( RtMidiError &error ) {
                qWarning() << &error.getMessage();
                return true;
            }
            _oxi_port_name_in = inPorts[oxiInIdx];
            qInfo() << "Port opened to " + inPorts[oxiInIdx];
            _in_idx = oxiInIdx;
            // TODO should also emit a signal to indicate that updates the UI.
            // But then both IN/OUT states may collide because they must not be the same.
        }
    }

    return false;
}
