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
    void runSendProject(void);
    void runFWUpdate(void);
    void runGetProject(void);
    void runGetAllProjects(void);

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;
    bool update_in_progress_ = false;

    QMidiIn midi_in;
    QMidiOut midi_out;
    std::vector<unsigned char> raw_data;
    std::vector<unsigned char> data_chunk;

    QString update_file_name_;

    QString worker_path_;
    QString project_path_;
    QString projects_path_;
    QString project_file_;

    OxiDiscovery *GetDiscovery();

public slots:
    void ui_DelayTimeUpdated(int value) {delay_time = value;}
    void onMidiReceive(QMidiMessage*);
    void OxiConnected();

    void SendRaw(void);
    void SendACK(void) {
        raw_data.clear();
        raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
        raw_data.push_back(MSG_CAT_FW_UPDATE);
        raw_data.push_back(MSG_FW_UPDT_ACK);
        raw_data.push_back(0xF7);
        SendRaw();
    };

    void SendCmd(OXI_SYSEX_CAT_e cat, uint8_t id);
    void SendBootExit(void);
    void SendUpdateReset(void);
    void GetFwVersion(void);
    void SendGotoBoot(OXI_SYSEX_FW_UPDT_e device_cmd);
    bool WaitForOXIUpdate(void);
    bool WaitProjectACK(void);

    bool UserCancelled(void);

    void ReadProjectFromFiles(void);


    void GetProject(void);
    // void GetSingleProject(void);
    // void GetAllProjects(void);
    void GetPattern(int pattern_idx);

    void ProcessProjectHeader(void);
    void ProcessPatternData(int pattern_idx);

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

    void UpdateProjIdx(int proj_idx) {
        if ((proj_idx > 0) && (proj_idx <= PROJ_NUM)) project_index_ = proj_idx - 1;
    };

    void DeleteProject(void) {
        raw_data.clear();
        raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
        raw_data.push_back(MSG_CAT_PROJECT);
        raw_data.push_back(MSG_PROJECT_DELETE_PROJECT);
        raw_data.push_back(project_index_);
        raw_data.push_back(0);
        raw_data.push_back(0xF7);
        SendRaw();
        emit ui_updateStatusLabel("ERASED!");
    }

    void GetCalibData(void) {
        SendCmd(MSG_CAT_SYSTEM, MSG_SYSTEM_GET_CALIB_DATA);
    }

    void setWorkerDirectory(const QString& workingDirectory) {
        worker_path_ = workingDirectory;

        QDir dir;
        projects_path_ = worker_path_ + "/Projects";
        qDebug() << "worker dir: " << projects_path_ << Qt::endl;
        if (!dir.exists(projects_path_)) {

            dir.mkdir(projects_path_);
        }
    }

signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void ui_UpdateProgressBar(int);
    void ui_UpdateError(void);
    void ui_ProjectError(void);
    void ui_Success(void);
    void ui_ConnectionError(void);
    void ui_lockUpdate(void);
    void ui_UpdateProjectProgressBar(int);
    void ui_updateStatusLabel(QString);
    void ResetFwVersion(void);
    void SetFwVersion(QString);
    void finished();
    void error(QString err);

public:
    enum WorkerState_e {
        WORKER_IDLE,
        WORKER_CANCELLING,
        // launch thread
        WORKER_FW_UPDATE_OXI_ONE,
        WORKER_FW_UPDATE_BLE,
        WORKER_FW_UPDATE_SPLIT,
        WORKER_GET_PROJECT,
        WORKER_SEND_PROJECT,
        WORKER_GET_ALL_PROJECTS,
    };

    void SetState(WorkerState_e state) {
        state_ = state;
    }

private:

    // typedef enum  {
    //     OXI_ONE_UPDATE,
    //     OXI_ONE_BLE_UPDATE,
    //     OXI_SPLIT_UPDATE,
        //     PROJECT_SEND,
        //     PROJECT_GET,
        // } process_e;

        // process_e run_process_;


    WorkerState_e state_ = WORKER_IDLE;

    int delay_time = 100;
    volatile int oxi_ack_ = 0;


    uint8_t seq_index_ = 0;
    // uint8_t pattern_index_ = 0;
    uint8_t project_index_ = 0;

    std::vector<uint8_t>sysex_data_;

    QFile file;
    Project project_;
    OxiDiscovery* _discovery;
};


#endif // MIDIWORKER_H
