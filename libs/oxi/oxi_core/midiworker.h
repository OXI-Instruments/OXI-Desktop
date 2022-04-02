#ifndef MIDIWORKER_H
#define MIDIWORKER_H

#include <QtWidgets>
#include "oxi_core_global.h"
#include "qmidiin.h"
#include "qmidiout.h"
#include "OXI_SYSEX_MSG.h"
#include "oxidiscovery.h"
#include "Project.h"

#include "MIDI.h"


class OXI_CORE_EXPORT MidiWorker : public QThread {
    Q_OBJECT

public:
    explicit MidiWorker(QObject *parent = 0, bool b = false);
    void run();
    void runSendProjectRAW(void);
    void runFWUpdate(void);

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;

    QMidiIn midi_in;
    QMidiIn midi_in_2;
    QMidiOut midi_out;
    std::vector<unsigned char> raw_data;
    std::vector<unsigned char> data_chunk;

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

    uint8_t project_index_ = 0;
    uint8_t seq_index_ = 0;
    uint8_t pattern_index_ = 0;

    OxiDiscovery *GetDiscovery();

public slots:
    void ui_DelayTimeUpdated(int value) {delay_time = value;}
    void onMidiReceive(QMidiMessage*);
    void LoadFile(void);

    void SendRaw(void);
    void SendACK(void) {
        raw_data.clear();
        raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
        raw_data.push_back(MSG_CAT_FW_UPDATE);
        raw_data.push_back(MSG_FW_UPDT_ACK);
        raw_data.push_back(0xF7);
        SendRaw();
    };


    void SendBootExit(void);
    void SendGotoBoot(OXI_SYSEX_FW_UPDT_e device_cmd);
    bool WaitForOXIUpdate(void);
    bool WaitProjectACK(void);

    void GetPattern(void);
//    void runSendProjectRAW(void);
    void ReadProjectFromFiles(void);
    void GetProject(void);

    void SetProjectHeader(uint16_t proj_index) {
        // clear sysex buffer
        raw_data.clear();
        // add OXI ONE sysex header
        raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
        // add command ids
        raw_data.push_back(MSG_CAT_PROJECT);
        raw_data.push_back(MSG_PROJECT_SEND_PROJ_HEADER);

        // add project slot index, OXI One will save project header and patterns in that slot
        raw_data.push_back(proj_index);
        // UNUSED in this case
        raw_data.push_back(0);
    };

    void SetPatternHeader(uint16_t proj_index, uint16_t patt_index) {
        // SET COMMAND HEADER
        raw_data.clear();
        raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
        raw_data.push_back(MSG_CAT_PROJECT);
        raw_data.push_back(MSG_PROJECT_SEND_PATTERN);
        raw_data.push_back(proj_index);
        // pattern slot, from 0 to 63
        raw_data.push_back(patt_index);
    };
//    void updateUpdateFile(QString update_file_name);

signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void ui_UpdateProgressBar(int);
    void ui_UpdateError(void);
    void ui_ConnectionError(void);
    void ui_UpdateMidiProgressBar(int);
    void ui_updateStatusLabel(QString);
    void finished();
    void error(QString err);

private:
    int delay_time = 100;
    int oxi_ack_ = 0;
    QFile file;
    Project project_;
    OxiDiscovery* _discovery;
};


#endif // MIDIWORKER_H
