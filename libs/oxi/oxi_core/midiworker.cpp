#include "midiworker.h"
#include "MIDI.h"
#include <QDebug>

#include <Nibble.h>
#include <crc32.h>
#include <FileTypes.h>
#include "oxidiscovery.h"
#include <QFile>
#include <QTextStream>
//#include <QTimer>


#include "SYSEX_APP.h"
//QString update_file_name_;


#define CANCELLED do { if (UserCancelled()) return; } while(0)

uint8_t seq_data_buffer[4096*2];

MidiWorker::MidiWorker(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
    _discovery = new OxiDiscovery(&midi_in, &midi_out);
    midi_in.setIgnoreTypes(false, true, true);

    //removed Q_ASSUME as is deprecated
    connect(&midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),
            this, SLOT(onMidiReceive(QMidiMessage*)));
    connect(_discovery, SIGNAL(discoveryOxiConnected()),
            this, SLOT(OxiConnected()));

    state_ = WORKER_IDLE;

}

OxiDiscovery* MidiWorker::GetDiscovery(){
    return _discovery;
}

void MidiWorker::OxiConnected(void) {

    emit ResetFwVersion();
    GetFwVersion();
}

void MidiWorker::SendRaw(void)
{
#define CHUNK_SIZE 4096
    int raw_data_len = raw_data.size();

    if (raw_data_len > CHUNK_SIZE) {

        for (int ibegin = 0; ibegin < raw_data_len; ibegin += CHUNK_SIZE) {
            int iend = ibegin + CHUNK_SIZE;
            if (iend > raw_data_len) iend = raw_data_len;
            data_chunk.assign(raw_data.begin() + ibegin, raw_data.begin() + iend);


            try {
                midi_out.sendRawMessage(data_chunk);
            }
            catch ( RtMidiError &error ) {
                error.printMessage();
                emit ui_ConnectionError();
            }

            QThread::msleep(100);
        }
    }
    else {
        try {
            midi_out.sendRawMessage(raw_data);
        }
        catch ( RtMidiError &error ) {
            error.printMessage();
            emit ui_ConnectionError();
        }
    }
}

bool MidiWorker::WaitProjectACK(void) {
    int timeout = 0;
    const int TIMEOUT_TIME = 3000;
    const int DELAY_TIME = 100;

    while((oxi_ack_ == 0) && (timeout < TIMEOUT_TIME))
    {
        QThread::msleep(DELAY_TIME);

        timeout += DELAY_TIME;
    }

    if (timeout >= TIMEOUT_TIME) {
        qDebug() << "TIMEOUT ERROR";
        return false;
    }
    // RESET FLAG ONCE USED
    oxi_ack_ = 0;
    return true;
}

bool MidiWorker::UserCancelled(void)
{
    if (state_ == WORKER_CANCELLING) {
        emit ui_UpdateProgressBar(0);

        emit ui_updateStatusLabel("CANCELLED");
        //qDebug() << "-------------user CANCELLED DENTRO IF--------------------------------";

        return 1;
    }
    //qDebug() << "-------------user CANCELLED FUERA IF--------------------------------";
    return 0;
}



void MidiWorker::GetFwVersion(void)
{
    SendCmd(MSG_CAT_SYSTEM, MSG_SYSTEM_SW_VERSION);
}

void MidiWorker::SendBootExit(void)
{
    SendCmd(MSG_CAT_FW_UPDATE, MSG_FW_UPDT_EXIT);
}

void MidiWorker::SendUpdateReset(void)
{
    SendCmd(MSG_CAT_FW_UPDATE, MSG_FW_UPDT_RESTART);
}

void MidiWorker::SendGotoBoot(OXI_SYSEX_FW_UPDT_e device_cmd)
{
    SendCmd(MSG_CAT_FW_UPDATE, device_cmd);
}

void MidiWorker::SendCmd(OXI_SYSEX_CAT_e cat, uint8_t id)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(cat);
    raw_data.push_back(id);
    raw_data.push_back(0xF7);
    try {
        midi_out.sendRawMessage(raw_data);
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        emit ui_UpdateError();
    }
}

bool MidiWorker::WaitForOXIUpdate(void)
{
    // when receiving the ack from bootloader, the update should start
    int retries = 10;

    while (((midi_out.isPortOpen() == false) ||
            (midi_in.isPortOpen() == false) ||
            (!_discovery->IsOutFwUpdate()) ||
            (!_discovery->IsInFwUpdate())) &&
           (retries > 0))
    {
        QThread::msleep(500);
        retries--;
    }

    if (retries > 0) {
        return true;
    } else {
        return false;
    }
}

// where the midiworker thread actually runs
void MidiWorker::run()
{
    qDebug() << "RUNNER \n\r";
    switch (this->state_) {
    case WORKER_FW_UPDATE_OXI_ONE:
    case WORKER_FW_UPDATE_SPLIT:
    case WORKER_FW_UPDATE_BLE:
        qDebug() << "Thread START: FW Update";
        runFWUpdate();
        break;
    case WORKER_SEND_PROJECT:
        qDebug() << "Thread START: Send Project RAW";
        runSendProject();
        break;
    case WORKER_GET_PROJECT:
        qDebug() << "Thread START: Get Project";
        runGetProject();
        break;
    case WORKER_GET_ALL_PROJECTS:
        qDebug() << "Thread START: Get ALL Projects";
        runGetAllProjects();
        break;
    default:
        break;
    }

    state_ = WORKER_IDLE;
    //a test to confirm thread stopped
    qDebug() << "Thread STOP\n";
}



// UPDATE PROCESS
void MidiWorker::runFWUpdate()
{
    int timeout = 0;
    int retries = 0;
    const int DELAY_TIME = 75;
    const int TIMEOUT_TIME = 3000;
    bool success = false;
    bool wait_for_ack = false;

    // state_ = WORKER_FW_UPDATE;

    file.setFileName(update_file_name_);
    if (!file.open(QIODevice::ReadOnly))
        return;

    switch (this->state_){
    case WORKER_FW_UPDATE_OXI_ONE:
        wait_for_ack = true;
        SendGotoBoot(MSG_FW_UPDT_OXI_ONE);

        if (WaitForOXIUpdate() == false) {
            emit ui_UpdateError();
            return;
        }
        break;
    case WORKER_FW_UPDATE_SPLIT:
        wait_for_ack = false;
        SendGotoBoot(MSG_FW_UPDT_OXI_SPLIT);
        QThread::msleep(500);
        break;
    case WORKER_FW_UPDATE_BLE:
        wait_for_ack = true;
        SendGotoBoot(MSG_FW_UPDT_OXI_BLE);
        QThread::msleep(500);
        break;
    default:
        break;
    }

    SendUpdateReset();
    this->msleep(200);

    QByteArray sysex_file_buffer = file.readAll();

    int package_num = 0;

    emit ui_UpdateProgressBar(0);
    update_in_progress_ = true;
    // todo remove
    emit  ui_lockUpdate();

    qDebug() << "File length: " << sysex_file_buffer.length() << Qt::endl;

    QList<QByteArray> packages = sysex_file_buffer.split(0xF7);
    for (package_num = 0; package_num < packages.size(); package_num ++)
    {
        packages[package_num].append(0xF7);
    }

LOOP:
    for (package_num = 0; package_num < packages.size(); )
    {
        timeout = 0;
        oxi_ack_ = 0;

        raw_data.assign(packages[package_num].constData(), packages[package_num].constData() + packages[package_num].size());

        try {
            midi_out.sendRawMessage(raw_data);
        }
        catch ( RtMidiError &error ) {
            error.printMessage();
            goto EXIT;
        }

        qDebug() << "Pck: " << package_num << " length: " << packages[package_num].size() << Qt::endl;

        emit ui_UpdateProgressBar(100 * package_num / packages.size());

        if (package_num >= packages.size() - 2) {
            success = true;
            emit ui_updateStatusLabel("SUCCESS!");
            goto EXIT;
        }

        if (wait_for_ack) {
            while((oxi_ack_ == 0) && (timeout < TIMEOUT_TIME))
            {
                this->msleep(DELAY_TIME);
                timeout += DELAY_TIME;
            }

            if (timeout >= TIMEOUT_TIME) {
                goto SEND_RESET;
            }
            else if (oxi_ack_ == MSG_FW_UPDT_ACK) {
                oxi_ack_ = 0;
                retries = 0;

                package_num ++;
            }
            else if (oxi_ack_ == MSG_FW_UPDT_NACK) {
                goto SEND_RESET;
            }
        }
        // Do not wait for ack, just sleep
        else {
            this->msleep(DELAY_TIME*6);

            package_num ++;
        }
    }

    goto EXIT;

SEND_RESET:
{
    SendUpdateReset();
    this->msleep(DELAY_TIME);

    if (retries < 3) {
        retries ++;
        goto LOOP;
    } else {
        goto EXIT;
    }

}

EXIT:
    if (success) {
        emit ui_UpdateProgressBar(100);

        switch (this->state_){
        case WORKER_FW_UPDATE_OXI_ONE:
            // do nothing
            break;
        case WORKER_FW_UPDATE_SPLIT:
            this->SendBootExit();
            break;
        case WORKER_FW_UPDATE_BLE:
            SendBootExit();
            break;
        default:
            break;

        }
        emit ui_Success();
    } else {
        emit ui_UpdateProgressBar(0);
        emit ui_UpdateError();
    }

    file.close();
}

/* SEND RAW DATA WITHOUT PARSING */
void MidiWorker::runSendProject(void)
{

    //qDebug() << "***********************Run send Project***********************\n";

    QFile projectFile( project_file_ );

    if ( projectFile.open(QIODevice::ReadOnly) )
    {
        emit ui_updateStatusLabel("SENDING");

        QByteArray buff = projectFile.readAll();

        SetProjectHeader(project_index_);
        Nibblize(raw_data, (uint8_t*)buff.data(), buff.size());
        // sysex end command
        raw_data.push_back(0xF7);

        oxi_ack_ = 0;

        SendRaw();

        // wait for OXI's ACK
        if (WaitProjectACK() != true) {
            qDebug() << "SEND PROJECT ERROR" << Qt::endl;
            emit ui_UpdateProgressBar(0);
            emit ui_ProjectError();
            return;
        }
        emit ui_UpdateProjectProgressBar(1);

        QFileInfo fi(project_file_);
        QString project_folder = fi.absolutePath();
        qDebug() << project_folder << Qt::endl;

        // 64 patterns in total
        for (int pattern_index = 0; pattern_index < 64; ++pattern_index)
        {

            QString pattern_file_ = project_folder + "/Pattern " + QString::number(pattern_index + 1) + FileExtension[FILE_PATTERN];

            QFile patternFile(pattern_file_);
            if ( patternFile.open(QIODevice::ReadOnly) )
            {
                qDebug() << "open: " << pattern_file_ << Qt::endl;

                buff = patternFile.readAll();

                SetPatternHeader(project_index_, pattern_index);
                Nibblize(raw_data, (uint8_t*)buff.data(), buff.size());
                raw_data.push_back(0xF7);

                oxi_ack_ = 0;

                // llamamos a qmidi para enviar el paquete de sysex
                SendRaw();

                if (WaitProjectACK() != true ) {
                    emit ui_UpdateProgressBar(0);
                    emit ui_UpdateError();
                    return;
                }

                CANCELLED;

                QString message = QString("Sending pattern %1...").arg(pattern_index + 1);
                emit ui_updateStatusLabel(message);
                emit ui_UpdateProjectProgressBar(100 * pattern_index / 64);
            }
        }

        emit ui_UpdateProjectProgressBar(100);
        emit ui_Success();
    }
    else {
        // emit ui_updateStatusLabel("PROJECT ERROR");
        emit ui_ProjectError();
    }

    //qDebug() << "***********************Run send Project ENDS***********************\n";
}

void MidiWorker::requestCancel() {
    cancelRequested = 1;  // Set the cancellation flag to 1 (true)
}

void MidiWorker::runGetProject(void)
//QMidiMessage* p_msg
{
    /*
     *  1. Request proj header
     *  2. wait until  PROJ HEADER received & check timeout
     *  ----CANCELLED();
     *  2.1. save project data into system
     *  2.2 Update progress bar on each step
     *  3. for 64 patterns
     *  - Request pattern
     *  - wait until pattern received
     *  - save pattern data into system
     *  - Update progress bar on each step
     *
     *  4. Update UI when process finished

 */

    GetProject();
    // wait for OXI's ACK
    if (WaitProjectACK() != true) {
        qDebug() << "GET PROJECT ERROR" << Qt::endl;
        emit ui_UpdateProgressBar(0);
        emit ui_ProjectError();
        return;
    }
    emit ui_UpdateProjectProgressBar(1);

    ProcessProjectHeader();

    // 64 patterns in total
    for (int pattern_index = 0; pattern_index < 64; ++pattern_index)
    {
        if (cancelRequested == 1) {  // Use the overloaded operator to check the value
            qDebug() << "Worker thread was requested to cancel.";
            return;  // Exit the run method early
        }


        GetPattern(pattern_index);

        if (WaitProjectACK() != true ) {
            emit ui_UpdateProgressBar(0);
            emit ui_UpdateError();
            return;
        }

        CANCELLED;

        ProcessPatternData(pattern_index);

        emit ui_UpdateProjectProgressBar(100 * pattern_index / 64);

    }


    // Project received successfully
    QDir dir = QDir::current(); // current directory
    QString absolutePath = dir.absoluteFilePath(project_path_);
    QString message = QString("Project saved on: %1").arg(absolutePath);
    emit ui_updateStatusLabel(message);

    //resetting value
    emit ui_UpdateProjectProgressBar(0);

    //flag to end cancel the button
    //emit workFinished(1);



}


void MidiWorker::runGetAllProjects(void){

    qDebug() << "\n------------\n runGetAllProjects\n------------\n ";

    setWorkerDirectory(worker_path_);

    //15 projects PROJ_NUM in total. Defined as 15 so should <= ?
    for(project_index_ = 0; project_index_ < PROJ_NUM; project_index_++){
        GetProject();
        // wait for OXI's ACK
        if (WaitProjectACK() != true) {
            qDebug() << "GET PROJECT ERROR" << Qt::endl;
            emit ui_UpdateProgressBar(0);
            emit ui_ProjectError();
            return;
        }
        emit ui_UpdateProjectProgressBar(1);

        ProcessProjectHeader();

        // 64 patterns in total
        for (int pattern_index = 0; pattern_index < 64; ++pattern_index)
        {
            GetPattern(pattern_index);

            if (WaitProjectACK() != true ) {
                emit ui_UpdateProgressBar(0);
                emit ui_UpdateError();
                return;
            }

            if (cancelRequested == 1) {  // Use the overloaded operator to check the value
                qDebug() << "Worker thread was requested to cancel.";
                return;  // Exit the run method early
            }

            CANCELLED; //this might not needed if using atomicInt

            ProcessPatternData(pattern_index);

            emit ui_UpdateProjectProgressBar(100 * pattern_index / 64);

        }

    }



    // Project received successfully
    QString message = QString("Projects backup finished!");
    emit ui_updateStatusLabel(message);

    QDir dir = QDir::current();
    //dir.cdUp(); // Move to the parent directory. DOES NOT SEEM TO WORK. using worker_path instead

    QString absolutePath = dir.absoluteFilePath(worker_path_);
    message = QString("Projects saved on: %1").arg(absolutePath);

    //resetting value
    emit ui_UpdateProjectProgressBar(0);
}

void MidiWorker::ReadProjectFromFiles(void)
{
    QFile projectFile( project_file_ );
    if ( projectFile.open(QIODevice::ReadOnly) )
    {
        QByteArray buff = projectFile.readAll();
        project_.readProject(buff);
        qDebug() << "Project: " << project_.project_data_.proj_name << Qt::endl;

        QFileInfo fi(project_file_);
        QString project_folder = fi.absolutePath();
        qDebug() << project_folder << Qt::endl;

        for (int pattern_index = 0; pattern_index < 64; ++pattern_index)
        {
            QString pattern_file_ = project_folder + "/Pattern " + QString::number(pattern_index + 1) + FileExtension[FILE_PATTERN];

            qDebug() << pattern_file_ << Qt::endl;

            QFile patternFile(pattern_file_);
            if ( patternFile.open(QIODevice::ReadOnly) )
            {
                buff = patternFile.readAll();
                project_.readPattern(buff, pattern_index);
            }
        }
    }
    else {
        // emit ui_updateStatusLabel("PROJECT ERROR");
        emit ui_ProjectError();
    }
}

void MidiWorker::GetProject(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_PROJECT);
    raw_data.push_back(MSG_PROJECT_GET_PROJ_HEADER);
    raw_data.push_back(project_index_);
    raw_data.push_back(0);
    raw_data.push_back(0xF7);

    ui_updateStatusLabel("");

    try {
        ui_updateStatusLabel("Receiving Project Data");
        midi_out.sendRawMessage(raw_data);
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
    }
}


void MidiWorker::ProcessProjectHeader(void)
{
    std::vector<uint8_t>buffer;

    // 8 + proj index + pattern index = 10
    DeNibblize(buffer, sysex_data_, 10);

    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    QDir system_dir;
    project_path_ = worker_path_ + "/Projects/Project " + QString::number(project_index_ + 1);
    if (!system_dir.exists(project_path_)) {

        system_dir.mkdir(project_path_);
    }


    QString proj_filename = project_path_ + "/Project " + QString::number(project_index_ + 1) + FileExtension[FILE_PROJECT];
    qDebug() << proj_filename << Qt::endl;;

    QFile proj_file( proj_filename );
    if ( proj_file.open(QIODevice::ReadWrite) )
    {
        proj_file.write(raw_data);
    }
    proj_file.close();
}

void MidiWorker::ProcessPatternData(int pattern_idx)
{
    std::vector<uint8_t>buffer;
    DeNibblize(buffer, sysex_data_, 10);

    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    QDir system_dir;
    if (!system_dir.exists(project_path_)) {
        system_dir.mkdir(project_path_);
    }

    QString patt_filename = project_path_ + "/Pattern " + QString::number(pattern_idx + 1) + FileExtension[FILE_PATTERN];
    qDebug() << patt_filename <<"\n";

    QFile patt_file( patt_filename );
    if ( patt_file.open(QIODevice::ReadWrite) )
    {
        patt_file.write(raw_data);
    }
    patt_file.close();
}


// void MidiWorker::GetSingleProject(void)
// {
//     state_ = WORKER_GET_PROJECT;
//     GetProject();
//     //runGetProject();
// }

// void MidiWorker::GetAllProjects(void)
// {
//     state_ = WORKER_GET_ALL_PROJECTS;
//     project_index_ = 0;

//     GetProject();
//     //runGetProject();
// }

void MidiWorker::GetPattern(int pattern_idx)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_PROJECT);
    raw_data.push_back(MSG_PROJECT_GET_PATTERN);
    raw_data.push_back(project_index_);
    raw_data.push_back(seq_index_ * 16 + pattern_idx);
    raw_data.push_back(0xF7);
    try {
        midi_out.sendRawMessage(raw_data);
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
    }
}

void MidiWorker::onMidiReceive(QMidiMessage* p_msg)
{
    if (p_msg->_status == MIDI_SYSEX)
    {
        if (memcmp(p_msg->getSysExData().data(), sysex_header, 6) == 0)
        {
            switch (p_msg->getSysExData()[6])
            {
            case MSG_CAT_FW_UPDATE:
                switch (p_msg->getSysExData()[7])
                {
                case MSG_FW_UPDT_READY:
                    //                    this->start();
                    break;
                case MSG_FW_UPDT_STARTED:
                    //                    this->start();
                    break;
                case MSG_FW_UPDT_ACK:
                    qDebug() << "ACK!!" << Qt::endl;
                    oxi_ack_ = MSG_FW_UPDT_ACK;
                    break;
                case MSG_FW_UPDT_NACK:
                    qDebug() << "NOT ACK" << Qt::endl;
                    oxi_ack_ = MSG_FW_UPDT_NACK;
                    break;
                default:
                    break;
                }

                break;
            case MSG_CAT_SYSTEM:
                switch (p_msg->getSysExData()[7])
                {
                case MSG_SYSTEM_SW_VERSION:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    DeNibblize(buffer, sysex_data, 8);

                    QString version;
                    for (size_t var = 0; var < buffer.size(); ++var) {
                        QChar c = QChar(buffer[var]);
                        if (c == ' ' || c == '\0') {
                            version.push_back('\0');
                            break;
                        } else {
                            version.push_back(c);
                        }
                    }

                    emit SetFwVersion(version);
                }
                break;
                case MSG_SYSTEM_HW_VERSION:
                    break;
                case MSG_SYSTEM_SEND_CALIB_DATA:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    // 8 + sector index
                    DeNibblize(buffer, sysex_data, 9);

                    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

                    QDir system_dir;
                    QString system_path = worker_path_ + "/System";
                    qDebug() << system_path << Qt::endl;
                    if (!system_dir.exists(system_path)) {

                        system_dir.mkdir(system_path);
                    }

                    QString calib_filename = system_path + "/calibration_data" + FileExtension[FILE_CALIBRATION];
                    QFile calib_file( calib_filename );
                    if ( calib_file.open(QIODevice::ReadWrite) )
                    {
                        calib_file.write(raw_data);
                    }
                    calib_file.close();

                    emit ui_UpdateProjectProgressBar(100);
                }
                break;
                default:
                    break;
                }

                break;
            case MSG_CAT_PROJECT:
                switch (p_msg->getSysExData()[7])
                {
                // Project HEADER DATA received from OXI ONE
                case MSG_PROJECT_SEND_PROJ_HEADER:
                {
                    sysex_data_ = p_msg->getSysExData();

                    oxi_ack_ = MSG_PROJECT_ACK;
                    break;
                }
                case MSG_PROJECT_GET_PROJ_HEADER:

                    break;
                // Get proj pattern from OXI
                case MSG_PROJECT_SEND_PATTERN:
                {
                    qDebug() << "PROJECT ACK" << Qt::endl;
                    oxi_ack_ = MSG_PROJECT_ACK;
                    sysex_data_ = p_msg->getSysExData();
                    break;
                }
                case MSG_PROJECT_GET_PATTERN:

                    break;
                case MSG_PROJECT_DELETE_PROJECT:

                    break;
                case MSG_PROJECT_DELETE_PATTERN:

                    break;
                case MSG_PROJECT_ACK:
                    qDebug() << "PROJECT ACK" << Qt::endl;
                    oxi_ack_ = MSG_PROJECT_ACK;
                    break;
                case MSG_PROJECT_NACK:
                    qDebug() << "PROJECT NOT ACK" << Qt::endl;
                    oxi_ack_ = MSG_PROJECT_NACK;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
    else if (p_msg->_status == MIDI_NOTE_ON)
    {
        qDebug() << "NOTE ON, ch: " <<  p_msg->_channel << "note: " << p_msg->_pitch << Qt::endl;
    }
    else if (p_msg->_status == MIDI_NOTE_OFF)
    {
        qDebug() << "NOTE OFF, ch: " <<  p_msg->_channel << "note: " << p_msg->_pitch << Qt::endl;
    }
}

