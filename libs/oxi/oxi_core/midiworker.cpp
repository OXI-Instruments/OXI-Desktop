#include "midiworker.h"
#include "MIDI.h"
#include "string.h"
#include <QDebug>

#include <Nibble.h>
#include <crc32.h>
#include "oxidiscovery.h"
#include <QFile>
#include <QTextStream>


#include "SYSEX_APP.h"
//QString update_file_name_;

uint8_t seq_data_buffer[4096*2];

MidiWorker::MidiWorker(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
    _discovery = new OxiDiscovery(&midi_in, &midi_out);
    midi_in.setIgnoreTypes(false, true, true);
    midi_in_2.setIgnoreTypes(false, true, true);
    Q_ASSUME(connect(&midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),
                     this, SLOT(onMidiReceive(QMidiMessage*))));

    // Listen to both OXI MIDI Ports just in case?
    // If so Receive msgs in the same callback
    Q_ASSUME(connect(&midi_in_2, SIGNAL(midiMessageReceived(QMidiMessage*)),
                     this, SLOT(onMidiReceive(QMidiMessage*))));

    QString desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QDir system_dir;
    oxi_path_ = desktop_path + "/OXI_Files";
    qDebug() << oxi_path_ << Qt::endl;
    if (!system_dir.exists(oxi_path_)) {

        system_dir.mkdir(oxi_path_);
    }

    projects_path_ = oxi_path_ + "/Projects";
    if (!system_dir.exists(projects_path_)) {

        system_dir.mkdir(projects_path_);
    }
}

OxiDiscovery* MidiWorker::GetDiscovery(){
    return _discovery;
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
    const int TIMEOUT_TIME = 5000;
    const int DELAY_TIME = 100;

    while((oxi_ack_ == 0) && (timeout < TIMEOUT_TIME))
    {
//        this->msleep(DELAY_TIME);
        QThread::msleep(DELAY_TIME);
        timeout += DELAY_TIME;
    }
    if (timeout >= TIMEOUT_TIME) {
        qDebug() << "TIMEOUT ERROR";
        return false;
    }
    return true;
}


void MidiWorker::SendBootExit(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_FW_UPDATE);
    raw_data.push_back(MSG_FW_UPDT_EXIT);
    raw_data.push_back(0xF7);
    try {
        midi_out.sendRawMessage(raw_data);
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
    }
}

void MidiWorker::SendGotoBoot(OXI_SYSEX_FW_UPDT_e device_cmd)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_FW_UPDATE);
    raw_data.push_back(device_cmd);
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

void MidiWorker::run()
{
    switch (this->run_process_) {
    case OXI_ONE_UPDATE:
    case OXI_SPLIT_UPDATE:
    case OXI_ONE_BLE_UPDATE:
        qDebug() << "Thread START: FW Update";
        runFWUpdate();
        break;
    case PROJECT_SEND:
        qDebug() << "Thread START: Send Project RAW";
        runSendProjectRAW();
        break;
    default:
        break;
    }
}


// UPDATE PROCESS
void MidiWorker::runFWUpdate()
{
    int timeout = 0;
    int retries = 0;
    const int DELAY_TIME = 50;
    const int TIMEOUT_TIME = 1000;
    bool success = false;
    bool wait_for_ack = false;

    file.setFileName(update_file_name_);
    if (!file.open(QIODevice::ReadOnly))
        return;

    switch (this->run_process_){
    case OXI_ONE_UPDATE:
        wait_for_ack = true;
        SendGotoBoot(MSG_FW_UPDT_OXI_ONE);

        if (WaitForOXIUpdate() == false) {
            emit ui_UpdateError();
            return;
        }
        break;
    case OXI_SPLIT_UPDATE:
        wait_for_ack = false;
        SendGotoBoot(MSG_FW_UPDT_OXI_SPLIT);
        QThread::msleep(500);
        break;
    case OXI_ONE_BLE_UPDATE:
        wait_for_ack = true;
        SendGotoBoot(MSG_FW_UPDT_OXI_BLE);
        QThread::msleep(500);
        break;
    default:
        break;
    }

    QByteArray sysex_file_buffer = file.readAll();

    int package_num = 0;

    emit ui_UpdateProgressBar(0);

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
            emit ui_updateStatusLabel("SUCESS!");
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
        uint8_t bootloader_reset[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_RESTART, 0xF7 };
        raw_data.assign (&bootloader_reset[0], &bootloader_reset[sizeof(bootloader_reset)]);

        try {
            midi_out.sendRawMessage( raw_data);
        }
        catch ( RtMidiError &error ) {
            error.printMessage();
        }
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

        switch (this->run_process_){
        case OXI_ONE_UPDATE:

            break;
        case OXI_SPLIT_UPDATE:
            SendBootExit();
            break;
        case OXI_ONE_BLE_UPDATE:
            SendBootExit();
            break;
        default:
            break;

        }

    } else {
        emit ui_UpdateProgressBar(0);
        emit ui_UpdateError();
    }

    file.close();
    this->quit();
//    this->terminate();
}

/* SEND RAW DATA WITHOUT PARSING */
void MidiWorker::runSendProjectRAW(void)
{

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
            emit ui_UpdateError();
            return;
        }
        emit ui_UpdateProjectProgressBar(1);

        QFileInfo fi(project_file_);
        QString project_folder = fi.absolutePath();
        qDebug() << project_folder << Qt::endl;

        for (int pattern_index = 0; pattern_index < 64; ++pattern_index)
        {
            QString pattern_file_ = project_folder + "/Pattern " + QString::number(pattern_index + 1) + ".bin";

            QFile patternFile(pattern_file_);
            if ( patternFile.open(QIODevice::ReadOnly) )
            {
                qDebug() << "open: " << pattern_file_ << Qt::endl;
                buff = patternFile.readAll();

                SetPatternHeader(project_index_, pattern_index);
                Nibblize(raw_data, (uint8_t*)buff.data(), buff.size());
                raw_data.push_back(0xF7);

                oxi_ack_ = 0;

                SendRaw();

                if (WaitProjectACK() != true) {
                    emit ui_UpdateProgressBar(0);
                    emit ui_UpdateError();
                    return;
                }

                emit ui_UpdateProjectProgressBar(100 * pattern_index / 64);
            }
        }

        emit ui_UpdateProjectProgressBar(100);
        emit ui_updateStatusLabel("SUCESS!");
    }
    else {
         emit ui_updateStatusLabel("PROJECT ERROR");
    }
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
            QString pattern_file_ = project_folder + "/Pattern " + QString::number(pattern_index + 1) + ".bin";

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
        emit ui_updateStatusLabel("PROJECT ERROR");
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

void MidiWorker::GetPattern(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_PROJECT);
    raw_data.push_back(MSG_PROJECT_GET_PATTERN);
    raw_data.push_back(project_index_);
    raw_data.push_back(seq_index_ * 16 + pattern_index_);
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
                    break;
                case MSG_SYSTEM_HW_VERSION:
                    break;
                case MSG_SYSTEM_SEND_CALIB_DATA:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    DeNibblize(buffer, sysex_data, 9);

                    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

                    QDir system_dir;
                    QString system_path = oxi_path_ + "/System";
                    qDebug() << system_path << Qt::endl;
                    if (!system_dir.exists(system_path)) {

                        system_dir.mkdir(system_path);
                    }

                    QString calib_filename = system_path + "/calibration_data.bin";
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
                case MSG_PROJECT_SEND_PROJ_HEADER:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    DeNibblize(buffer, sysex_data, 10);

                    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

                    QDir system_dir;
                    project_path_ = oxi_path_ + "/Projects/Project " + QString::number(project_index_ + 1);
                    if (!system_dir.exists(project_path_)) {

                        system_dir.mkdir(project_path_);
                    }

                    QString proj_filename = project_path_ + "/Project " + QString::number(project_index_ + 1) + ".bin";
                    qDebug() << proj_filename << Qt::endl;;

                    QFile proj_file( proj_filename );
                    if ( proj_file.open(QIODevice::ReadWrite) )
                    {
                        proj_file.write(raw_data);
                    }
                    proj_file.close();

                    pattern_index_ = 0;

                    GetPattern();
                    break;
                }
                case MSG_PROJECT_GET_PROJ_HEADER:

                    break;
                case MSG_PROJECT_SEND_PATTERN:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    DeNibblize(buffer, sysex_data, 10);

                    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

                    QDir system_dir;
                    if (!system_dir.exists(project_path_)) {

                        system_dir.mkdir(project_path_);
                    }

                    QString patt_filename = project_path_ + "/Pattern " + QString::number(pattern_index_ + 1) + ".bin";
                    qDebug() << patt_filename;

                    QFile patt_file( patt_filename );
                    if ( patt_file.open(QIODevice::ReadWrite) )
                    {
                        patt_file.write(raw_data);
                    }
                    patt_file.close();

                    pattern_index_ ++;

                    GetPattern();

                    emit ui_UpdateProjectProgressBar(100 * pattern_index_ / 64);
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

