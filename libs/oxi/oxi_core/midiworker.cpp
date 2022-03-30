#include "midiworker.h"
#include "MIDI.h"
#include "string.h"
#include <QDebug>
#include <Nibble.h>
#include <crc32.h>

#include <QFile>
#include <QTextStream>


#include "SYSEX_APP.h"
//QString update_file_name_;

uint8_t seq_data_buffer[4096*2];





MidiWorker::MidiWorker(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
    midi_in.setIgnoreTypes(false, true, true);
    Q_ASSUME(connect(&midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),
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

void MidiWorker::SendRaw(void)
{
    try {
        midi_out.sendRawMessage(raw_data);
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
         emit ui_ConnectionError();
    }
}


void MidiWorker::LoadFile(void)
{
//    update_file_name_ = QFileDialog::getOpenFileName(
//                this,
//                tr("Select SYSEX"),
//                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
//                tr("Sysex ( *.syx);All Files ( * )"));
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
QString fw_update = QString("FW Update");
    int retries = 10;

    while (((midi_out.isPortOpen() == false) ||
           (midi_in.isPortOpen() == false) ||
           (port_out_string.contains(fw_update) == false) ||
           (port_in_string.contains(fw_update) == false)) &&
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
    int timeout = 0;
    int retries = 0;
    const int DELAY_TIME = 50;
    const int TIMEOUT_TIME = 1000;
    bool success = false;
    bool wait_for_ack = false;

    file.setFileName(update_file_name_);
    if (!file.open(QIODevice::ReadOnly))
        return;

    switch (this->updated_device_){
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
        sysex_ack_ = 0;

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
            while((sysex_ack_ == 0) && (timeout < TIMEOUT_TIME))
            {
                this->msleep(DELAY_TIME);
                timeout += DELAY_TIME;
            }

            if (timeout >= TIMEOUT_TIME) {
                goto SEND_RESET;
            }
            else if (sysex_ack_ == MSG_FW_UPDT_ACK) {
                sysex_ack_ = 0;
                retries = 0;

                package_num ++;
            }
            else if (sysex_ack_ == MSG_FW_UPDT_NACK) {
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

        switch (this->updated_device_){
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

void MidiWorker::WorkerRefreshDevices(void)
{
    //        QList<QString> midi_list = midi_out.devices().values();

    //        ui->comboBox->clear();

    //        for (int i = 0; i != midi_out.devices().size(); ++i)
    //        {
    //            ui->comboBox->addItem(midi_list[i], i);
    //        }
}

void MidiWorker::SendProject(void)
{
//    raw_data.clear();
//    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
//    raw_data.push_back(MSG_CAT_PROJECT);
//    raw_data.push_back(MSG_PROJECT_GET_PROJ_HEADER);
//    raw_data.push_back(project_index);
//    raw_data.push_back(0);
//    raw_data.push_back(0xF7);
//    midi_out.sendRawMessage(raw_data);
}

void MidiWorker::GetProject(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_PROJECT);
    raw_data.push_back(MSG_PROJECT_GET_PROJ_HEADER);
    raw_data.push_back(project_index);
    raw_data.push_back(0);
    raw_data.push_back(0xF7);
    try {
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
    raw_data.push_back(project_index);
    raw_data.push_back(seq_index * 16 + pattern_index);
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
                    sysex_ack_ = MSG_FW_UPDT_ACK;
                    break;
                case MSG_FW_UPDT_NACK:
                    sysex_ack_ = MSG_FW_UPDT_NACK;
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

                    emit ui_UpdateMidiProgressBar(100);
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
                    project_path_ = oxi_path_ + "/Projects/Project " + QString::number(project_index + 1);
                    if (!system_dir.exists(project_path_)) {

                        system_dir.mkdir(project_path_);
                    }

                    QString proj_filename = project_path_ + "/Project " + QString::number(project_index + 1) + ".bin";
                    qDebug() << proj_filename << Qt::endl;;

                    QFile proj_file( proj_filename );
                    if ( proj_file.open(QIODevice::ReadWrite) )
                    {
                        proj_file.write(raw_data);
                    }
                    proj_file.close();

                    pattern_index = 0;

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

                    QString patt_filename = project_path_ + "/Pattern " + QString::number(pattern_index + 1) + ".bin";
                    qDebug() << patt_filename;

                    QFile patt_file( patt_filename );
                    if ( patt_file.open(QIODevice::ReadWrite) )
                    {
                        patt_file.write(raw_data);
                    }
                    patt_file.close();

                    pattern_index ++;

                    GetPattern();

                    emit ui_UpdateMidiProgressBar(100 * pattern_index / 64);
                    break;
                }
                case MSG_PROJECT_GET_PATTERN:

                    break;
                case MSG_PROJECT_DELETE_PROJECT:

                    break;
                case MSG_PROJECT_DELETE_PATTERN:

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
}

