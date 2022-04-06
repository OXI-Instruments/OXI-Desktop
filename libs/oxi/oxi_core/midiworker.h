#ifndef MIDIWORKER_H
#define MIDIWORKER_H

#include <QtWidgets>
#include "oxi_core_global.h"
#include "qmidiin.h"
#include "qmidiout.h"
#include "OXI_SYSEX_MSG.h"
#include "oxidiscovery.h"
#include "Project.h"

#include "Project.h"


class OXI_CORE_EXPORT MidiWorker : public QThread {
    Q_OBJECT

public:
    explicit MidiWorker(QObject *parent = 0, bool b = false);
    void run();

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;

    QMidiIn midi_in;
    QMidiIn midi_in_2;
    QMidiOut midi_out;
    std::vector<unsigned char> raw_data;
    QString update_file_name_;
    QString oxi_path_;
    QString project_path_;
    QString projects_path_;
    QString project_file_;

    typedef enum  {
        OXI_ONE_UPDATE,
        OXI_ONE_BLE_UPDATE,
        OXI_SPLIT_UPDATE,

        PROJECT_SEND,
    } process_e;

    process_e run_process_;

    uint8_t project_index = 0;
    uint8_t seq_index = 0;
    uint8_t pattern_index = 0;

    OxiDiscovery *GetDiscovery();

public slots:
    void ui_DelayTimeUpdated(int value) {delay_time = value;}
    void onMidiReceive(QMidiMessage*);
    void LoadFile(void);

    void SendRaw(void);
    void SendACK(void);


    void SendBootExit(void);
    void SendGotoBoot(OXI_SYSEX_FW_UPDT_e device_cmd);
    bool WaitForOXIUpdate(void);

    void GetPattern(void);
    void SendProject(void);
    void GetProject(void);
//    void updateUpdateFile(QString update_file_name);

signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void ui_UpdateProgressBar(int);
    void ui_UpdateError(void);
    void ui_ConnectionError(void);
    void ui_UpdateProjectProgressBar(int);
    void ui_updateStatusLabel(QString);
    void finished();
    void error(QString err);

private:
    int delay_time = 100;
    int sysex_ack_ = 0;
    QFile file;
    Project project_;
    OxiDiscovery* _discovery;
};


#endif // MIDIWORKER_H
