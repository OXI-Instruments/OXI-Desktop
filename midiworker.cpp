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

    desktop_path_ = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
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
            goto EXIT;
        }

        if (this->wait_for_ack) {
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
    } else {
        emit ui_UpdateProgressBar(0);
    }

    file.close();
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
                    this->Stop = false;
                    this->start();
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
                    QString system_path = desktop_path_ + "/OXI_Files/System/";
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

                    uint16_t proj_index = sysex_data.at(8) + 1;

                    DeNibblize(buffer, sysex_data, 10);

                    void * p_void = malloc(buffer.size());
                    PROJ_buffer_s buf;
                    PROJ_buffer_s * p_proj_buf = static_cast<PROJ_buffer_s *>(p_void);
                    uint8_t * p_buf = reinterpret_cast<uint8_t*>(&buf);
                    std::copy(buffer.begin(), buffer.end(), (uint8_t*)p_void);

                    int buf_len = (sizeof(PROJ_buffer_s));
                    buf_len = (buf_len - 4) / 4;

                    uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(p_void), buf_len);

                    if (crc_check == p_proj_buf->crc_check)
                    {
                        qDebug() << "PROJ received ok" << Qt::endl;
                    }
                    raw_data.clear();
                    raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
                    raw_data.push_back(MSG_CAT_PROJECT);
                    raw_data.push_back(MSG_PROJECT_SEND_PROJ_HEADER);

                    raw_data.push_back(proj_index);
                    raw_data.push_back(0);

                    Nibblize(raw_data, (uint8_t*)p_void, sizeof(PROJ_buffer_s));

                    raw_data.push_back(0xF7);
                    midi_out.sendRawMessage(raw_data);

                    break;
                }
                case MSG_PROJECT_GET_PROJ_HEADER:

                    break;
                case MSG_PROJECT_SEND_PATTERN:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    uint16_t proj_index = sysex_data.at(8);
                    uint16_t pattern_index = sysex_data.at(9);
                    uint16_t seq_index = pattern_index / 16;
                    pattern_index = pattern_index % 16;

                    DeNibblize(buffer, sysex_data, 10);

                    uint8_t seq_type = buffer.at(0);

                    DeNibblize(buffer, sysex_data, 9);

                    QByteArray raw_data(reinterpret_cast<const char*>(buffer.data()), buffer.size());

                    QDir system_dir;
                    QString system_path = desktop_path_ + "/OXI_Files";
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

