#ifndef WORKER_H
#define WORKER_H

#include <QtWidgets>
#include "qmidiin.h"
#include "qmidiout.h"
#include "OXI_SYSEX_MSG.h"



class Worker : public QThread {
    Q_OBJECT

public:
    explicit Worker(QObject *parent = 0, bool b = false);
    void run();

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;

    QMidiIn midi_in;
    QMidiOut midi_out;
    std::vector<unsigned char> raw_data;
    QString update_file_name_;

    QString port_in_string;
    QString port_out_string;
    int port_out_index;
    int port_in_index;

public slots:
    void ui_DelayTimeUpdated(int value) {delay_time = value;}
    void WorkerRefreshDevices(void);
    void onMidiReceive(QMidiMessage*);
//    void updateUpdateFile(QString update_file_name);

signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void ui_UpdateProgressBar(int);
    void finished();
    void error(QString err);

private:
    int delay_time = 100;
    int sysex_ack_ = 0;
    QFile file;
};


#endif // WORKER_H
