#include "midiworker.h"
#include "MIDI.h"
#include "string.h"
#include <QDebug>
#include <mainwindow.h>
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

void MidiWorker::LoadFile(void)
{
    update_file_name_ = QFileDialog::getOpenFileName(
                nullptr,
                tr("Select SYSEX"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                tr("Sysex ( *.syx);All Files ( * )"));
}

void MidiWorker::SendBootExit(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_FW_UPDATE);
    raw_data.push_back(MSG_FW_UPDT_EXIT);
    raw_data.push_back(0xF7);
    midi_out.sendRawMessage(raw_data);
}

// run() will be called when a thread starts
void MidiWorker::run()
{
    file.setFileName(update_file_name_);
    if (!file.open(QIODevice::ReadOnly))
        return;

    int timeout = 0;
    int retries = 0;
    const int DELAY_TIME = 50;
    const int TIMEOUT_TIME = 1000;
    bool success = false;
    bool wait_for_ack = false;

    switch (this->updated_device_){
    case OXI_ONE_UPDATE:
        wait_for_ack = true;
        break;
    case OXI_SPLIT_UPDATE:
        wait_for_ack = false;
        break;
    case OXI_ONE_BLE_UPDATE:
        wait_for_ack = true;
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

void MidiWorker::GetProject(void)
{
    raw_data.clear();
    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    raw_data.push_back(MSG_CAT_PROJECT);
    raw_data.push_back(MSG_PROJECT_GET_PROJ_HEADER);
    raw_data.push_back(project_index);
    raw_data.push_back(0);
    raw_data.push_back(0xF7);
    midi_out.sendRawMessage(raw_data);
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
    midi_out.sendRawMessage(raw_data);
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


#if 0
                    if (seq_type == SEQ_MULTITRACK) {
                        void * p_void = malloc(buffer.size());
                        SEQ_multi_buffer_s buf;
                        SEQ_multi_buffer_s * p_multi_buf = static_cast<SEQ_multi_buffer_s *>(p_void);
                        uint8_t * p_buf = reinterpret_cast<uint8_t*>(&buf);
                        std::copy(buffer.begin(), buffer.end(), (uint8_t*)p_void);

                        int buf_len = (sizeof(SEQ_multi_buffer_s));
                        buf_len = (buf_len - 4) / 4;

                        uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(p_void), buf_len);

                        if (crc_check == p_multi_buf->crc_check) {
                            qDebug() << "MULTI received ok" << Qt::endl;
                            raw_data.clear();
                            raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
                            raw_data.push_back(MSG_CAT_PROJECT);
                            raw_data.push_back(MSG_PROJECT_SEND_PATTERN);

                            uint8_t project_index = 0;
                            uint8_t seq_index = 0;
                            uint8_t pattern_index = 2;

                            raw_data.push_back(project_index);
                            raw_data.push_back(seq_index * 16 + pattern_index);

                            Nibblize(raw_data, (uint8_t*)p_void, sizeof(SEQ_multi_buffer_s));

                            QString voidfilename ="/Users/ManuelVr/Desktop/void.txt";
                            QFile file_void( voidfilename );
                            if ( file_void.open(QIODevice::ReadWrite) )
                            {
                                int j = 0;
                                uint8_t * p_data = (uint8_t*)p_void;
                                QTextStream stream( &file_void );
                                do {
                                    for (int i=0; i< 32; i++){
                                        if (p_data[j] < 0x10) {
                                            stream << "0" << p_data[j] << Qt::hex << " ";
                                        } else {
                                            stream << p_data[j] << Qt::hex << " ";
                                        }
                                        j++;
                                    }
                                    stream << Qt::endl;
                                } while (j < sizeof(SEQ_multi_buffer_s));
                            }

                            raw_data.push_back(0xF7);
                            midi_out.sendRawMessage(raw_data);

                        }
                        else {
                            qDebug() << "CRC Invalid calculated:" << crc_check << " Received: "<< buf.crc_check << Qt::endl;

                        }
                        free(p_void);
                    }
                    else if (seq_type == SEQ_MONO) {
                        SEQ_mono_buffer_s buf;
                        uint8_t * p_buf =reinterpret_cast<uint8_t*>(&buf);
                        std::copy(buffer.begin(), buffer.end(), p_buf);

                        int buf_len = (sizeof(SEQ_mono_buffer_s)); // 1608
                        buf_len = (buf_len - 4) / 4;

                        uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(&buf), buf_len);

                        if (crc_check == buf.crc_check) {
                            qDebug() << "MONO received ok" << Qt::endl;

                            raw_data.clear();
                            raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
                            raw_data.push_back(MSG_CAT_PROJECT);
                            raw_data.push_back(MSG_PROJECT_SEND_PATTERN);

                            uint8_t project_index = 0;
                            uint8_t seq_index = 0;
                            uint8_t pattern_index = 3;

                            raw_data.push_back(project_index);
                            raw_data.push_back(seq_index * 16 + pattern_index);

                            Nibblize(raw_data, (uint8_t*)&buf, sizeof(SEQ_mono_buffer_s));

                            raw_data.push_back(0xF7);
                            midi_out.sendRawMessage(raw_data);
                        }
                        else {
                            return;
                        }
                    }
                    else if (seq_type == SEQ_CHORDS) {
                        SEQ_chord_buffer_s buf;
                        uint8_t * p_buf =reinterpret_cast<uint8_t*>(&buf);
                        std::copy(buffer.begin(), buffer.end(), p_buf);

                        int buf_len = (sizeof(SEQ_chord_buffer_s)); // 1872
                        buf_len = (buf_len - 4) / 4;

                        uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(&buf), buf_len);

                        if (crc_check == buf.crc_check) {
                            qDebug() << "CHORD received ok" << Qt::endl;

                            QString voidfilename ="/Users/ManuelVr/Desktop/chords.txt";
                            QFile file_void( voidfilename );
                            if ( file_void.open(QIODevice::ReadWrite) )
                            {
                                int j = 0;
                                uint8_t * p_data = (uint8_t*)&buf;
                                QTextStream stream( &file_void );
                                do {
                                    for (int i=0; i< 32; i++){
                                        if (p_data[j] < 0x10) {
                                            stream << "0" << p_data[j] << Qt::hex << " ";
                                        } else {
                                            stream << p_data[j] << Qt::hex << " ";
                                        }
                                        j++;
                                    }
                                    stream << Qt::endl;
                                } while (j < sizeof(SEQ_chord_buffer_s));
                            }


                            raw_data.clear();
                            raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
                            raw_data.push_back(MSG_CAT_PROJECT);
                            raw_data.push_back(MSG_PROJECT_SEND_PATTERN);

                            uint8_t project_index = 0;
                            uint8_t seq_index = 0;
                            uint8_t pattern_index = 4;

                            raw_data.push_back(project_index);
                            raw_data.push_back(seq_index * 16 + pattern_index);

                            Nibblize(raw_data, (uint8_t*)&buf, sizeof(SEQ_chord_buffer_s));
                            raw_data.push_back(0xF7);
                            midi_out.sendRawMessage(raw_data);
                        }
                        else {
                            return;
                        }
                    }
                    else if (seq_type == SEQ_POLY) {
                        SEQ_poly_buffer_s buf;
                        uint8_t * p_buf =reinterpret_cast<uint8_t*>(&buf);
                        std::copy(buffer.begin(), buffer.end(), p_buf);

                        int buf_len = (sizeof(SEQ_poly_buffer_s));
                        buf_len = (buf_len - 4) / 4;

                        uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(&buf), buf_len);

                        if (crc_check == buf.crc_check) {
                            qDebug() << "POLY received ok" << Qt::endl;

                            QString voidfilename ="/Users/ManuelVr/Desktop/poly.txt";
                            QFile file_void( voidfilename );
                            if ( file_void.open(QIODevice::ReadWrite) )
                            {
                                int j = 0;
                                uint8_t * p_data = (uint8_t*)&buf;
                                QTextStream stream( &file_void );
                                do {
                                    for (int i=0; i< 32; i++){
                                        if (p_data[j] < 0x10) {
                                            stream << "0" << p_data[j] << Qt::hex << " ";
                                        } else {
                                            stream << p_data[j] << Qt::hex << " ";
                                        }
                                        j++;
                                    }
                                    stream << Qt::endl;
                                } while (j < sizeof(SEQ_poly_buffer_s));
                            }

                            raw_data.clear();
                            raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
                            raw_data.push_back(MSG_CAT_PROJECT);
                            raw_data.push_back(MSG_PROJECT_SEND_PATTERN);

                            uint8_t project_index = 0;
                            uint8_t seq_index = 0;
                            uint8_t pattern_index = 5;

                            raw_data.push_back(project_index);
                            raw_data.push_back(seq_index * 16 + pattern_index);

                            Nibblize(raw_data, (uint8_t*)&buf, sizeof(SEQ_poly_buffer_s));
                            raw_data.push_back(0xF7);
                            midi_out.sendRawMessage(raw_data);
                        }
                        else {
                            return;
                        }
                    }
#endif
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

