#ifndef QMIDIIN_H
#define QMIDIIN_H

#include <QStringList>
#include <QObject>
#include "RtMidi.h"
#include "qmidimessage.h"


class QMidiIn : public QObject
{
    Q_OBJECT
public:
    explicit QMidiIn(QObject *parent = 0);
    QStringList getPorts();
    unsigned int getPortCount();
    void openPort(QString name);
    void openPort(unsigned int index);
    void openVirtualPort(QString name);
    void closePort();
    bool isPortOpen();
    void setIgnoreTypes(bool sysex = false, bool time = true, bool sense = true);
private:
    void onMidiMessageReceive(QMidiMessage *msg);
    static void callback( double deltatime, std::vector< unsigned char > *message, void *userData );

private:
    RtMidiIn *_midiIn;

signals:
    void midiMessageReceived(QMidiMessage *message);

public slots:
};

#endif // QMIDIIN_H
