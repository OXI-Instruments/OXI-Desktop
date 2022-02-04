#ifndef MIDIWORKER_H
#define MIDIWORKER_H

#include <QtWidgets>
#include "qmidiin.h"
#include "qmidiout.h"
#include "OXI_SYSEX_MSG.h"



class MidiWorker : public QThread {
    Q_OBJECT

public:
    explicit MidiWorker(QObject *parent = 0, bool b = false);
    void run();

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;

    QMidiIn midi_in;
    QMidiOut midi_out;
    std::vector<unsigned char> raw_data;
    QString update_file_name_;
    QString project_path_;

    QString port_in_string;
    QString port_out_string;
    int port_out_index;
    int port_in_index;
    bool wait_for_ack = true;

    uint8_t project_index = 0;
    uint8_t seq_index = 0;
    uint8_t pattern_index = 0;

public slots:
    void ui_DelayTimeUpdated(int value) {delay_time = value;}
    void WorkerRefreshDevices(void);
    void onMidiReceive(QMidiMessage*);

    void GetPattern(void);
    void GetProject(void);
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
    QString desktop_path_;
};


#endif // MIDIWORKER_H
