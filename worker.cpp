#include "worker.h"
#include "MIDI.h"
#include "string.h"
#include <QDebug>
#include <mainwindow.h>

#include <QFile>
#include <QTextStream>


#include "SYSEX_APP.h"
//QString update_file_name_;

uint8_t seq_data_buffer[4096*2];


uint32_t crc32(uint32_t * data, int data_len)
{
    const uint32_t POLY = 0x4C11DB7;
    uint32_t init = 0XFFFFFFFF;
    uint32_t crc_ = 0;
    uint32_t data_ = 0;

    for (int index = 0; index < data_len; index++) {
        data_ = data[index];
         crc_ = init ^ data_;
        for (int i = 0; i < 32 ; i++) {
            if ((0x80000000 & crc_) != 0) {
                crc_ = (crc_ << 1) ^ POLY;
            } else {
                crc_ = (crc_ << 1);
            }
        }

//        qDebug() << "Data[" << index << "]:  " << Qt::hex << data_ <<Qt::endl;
//        qDebug() << "Crc: " << Qt::hex << crc_ <<Qt::endl;
        init = crc_;
    }

    return crc_;
}


Worker::Worker(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
    midi_in.setIgnoreTypes(false, true, true);
    Q_ASSUME(connect(&midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),
                     this, SLOT(onMidiReceive(QMidiMessage*))));
}

// run() will be called when a thread starts
void Worker::run()
{
#if 0
    int i = 0;
    while(!this->Stop)
    {
        QMutex mutex;
        // prevent other threads from changing the "Stop" value
        mutex.lock();
        if(this->Stop) break;
        mutex.unlock();

        // emit the signal for the count label
        emit ui_UpdateProgressBar(i++);

        if (i == 50) i = 0;
        // slowdown the count change, msec
        this->msleep(delay_time);
    }
    return;
#endif

#if 1
    //        update_file_name_ = QFileDialog::getOpenFileName(
    //                    (QWidget*)this,
    //                    tr("Select SYSEX"),
    //                    tr("/Users/ManuelVr/Desktop"),
    //                    tr("Sysex ( *.syx);All Files ( * )"));
    //    update_file_name_ = "/Users/ManuelVr/Desktop/midi_ble.syx";

    file.setFileName(update_file_name_);
    if (!file.open(QIODevice::ReadOnly))
        return;

    //    QByteArray sysex_file_buffer;
    //    sysex_file_buffer.setRawData(file.readAll(), file.size());
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
#endif
}

void Worker::WorkerRefreshDevices(void)
{
    //        QList<QString> midi_list = midi_out.devices().values();

    //        ui->comboBox->clear();

    //        for (int i = 0; i != midi_out.devices().size(); ++i)
    //        {
    //            ui->comboBox->addItem(midi_list[i], i);
    //        }
}


void DeNibblize(std::vector<uint8_t> &buf, std::vector<uint8_t> &sysex_data)
{
    int count = 0;
    for (auto it = sysex_data.begin(); it != sysex_data.end(); ++it)
      {
        uint8_t byte = static_cast<uint8_t>(*it);

         if ((count & 1) == 0)
         {
             buf.push_back(byte & 0xf);
         } else {
             int size = buf.size();
             buf[size] |= (byte << 4);
         }
         count++;
      }
}


void DeNibblize2(std::vector<uint8_t> &buf, std::vector<uint8_t> &data, int offset)
{
    int count = 0;
    for (uint32_t it = offset; it < data.size(); ++it)
    {
        uint8_t byte = data.at(it);

        if ((count & 1) == 0)
        {
            uint8_t byte_ = (byte << 4) & 0xF0; // byte high
            buf.push_back(byte_);
        } else {
            int size = buf.size() - 1;
            buf[size] |= byte & 0xF;
        }
        count++;
    }
}

void Nibblize2(std::vector<uint8_t> &buf, std::vector<uint8_t> &data, int offset)
{
    int count = 0;
    for (uint32_t it = offset; it < data.size(); ++it)
    {

        uint8_t byte = data.at(it);
        uint8_t byte_h = (byte << 4) & 0xF0;
        buf.push_back(byte_h);
        uint8_t byte_l = byte & 0xF;
        buf.push_back(byte_l);
    }
}

void Nibblize(std::vector<uint8_t> &buf, uint8_t data[], int length)
{
    int count = 0;
    for (int it = 0; it < length; ++it)
    {

        uint8_t byte = data[it];
        uint8_t byte_h = (byte >> 4) & 0x0F;
        buf.push_back(byte_h);
        uint8_t byte_l = byte & 0xF;
        buf.push_back(byte_l);
    }
}


void Worker::onMidiReceive(QMidiMessage* p_msg)
{
    if (p_msg->_status == MIDI_SYSEX)
    {
        //        char * p_SysExData = p_msg->getSysExData().data();
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
            case MSG_CAT_PROJECT:
                switch (p_msg->getSysExData()[7])
                {
                case MSG_PROJECT_GET_PROJ_HEADER:
   {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    uint16_t proj_index = sysex_data.at(8);
                    uint16_t pattern_index = sysex_data.at(9);
                    uint16_t seq_index = pattern_index / 16;
                    pattern_index = pattern_index % 16;

                    DeNibblize2(buffer, sysex_data, 10);

                    uint8_t seq_type = buffer.at(0);


                    void * p_void = malloc(buffer.size());
                    PROJ_buffer_s buf;
                    PROJ_buffer_s * p_proj_buf = static_cast<PROJ_buffer_s *>(p_void);
                    uint8_t * p_buf = reinterpret_cast<uint8_t*>(&buf);
                    std::copy(buffer.begin(), buffer.end(), (uint8_t*)p_void);

                    int buf_len = (sizeof(PROJ_buffer_s));
                    buf_len = (buf_len - 4) / 4;

                    uint32_t crc_check = crc32(reinterpret_cast<uint32_t*>(p_void), buf_len);

                    if (crc_check == p_proj_buf->crc_check) {
                        qDebug() << "PROJ received ok" << Qt::endl;
                    }

                    break;
                }
                case MSG_PROJECT_SEND_PROJ_HEADER:

                    break;
                case MSG_PROJECT_GET_PATTERN:
                {
                    std::vector<uint8_t>buffer;
                    std::vector<uint8_t>sysex_data = p_msg->getSysExData();

                    uint16_t proj_index = sysex_data.at(8);
                    uint16_t pattern_index = sysex_data.at(9);
                    uint16_t seq_index = pattern_index / 16;
                    pattern_index = pattern_index % 16;

                    DeNibblize2(buffer, sysex_data, 10);

                    uint8_t seq_type = buffer.at(0);


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

#if 0
                            QString Hfilename="/Users/ManuelVr/Desktop/asdf.txt";
                            QFile fileH( Hfilename );
                            if ( fileH.open(QIODevice::ReadWrite) )
                            {
                                int j = 0;
                                QTextStream stream( &fileH );
                                do {
                                    for (int i=0; i< 32; i++){
                                        if (buffer[j] < 0x10) {
                                            stream << "0" << buffer[j] << Qt::hex << " ";
                                        } else {
                                            stream << buffer[j] << Qt::hex << " ";
                                        }

//                                        stream << "1: " << Qt::showbase << buffer[i] << Qt::hex << " " << Qt::endl;
//                                        stream << "2: " << QString::number(buffer[i], 16) << Qt::hex << " " << Qt::endl;
//                                        stream << "3: " << (void *)buffer[i] << Qt::hex << " " << Qt::endl;
//                                        QString valueInHex= QString("%1").arg(buffer[i] , 0, 16);
//                                        stream << "4: " << (void *)buffer[i] << Qt::hex << " " << Qt::endl;
                                        j++;
                                    }
                                    stream << Qt::endl;
                                } while (j < buffer.size());
                            }
#endif
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

//                                        stream << "1: " << Qt::showbase << buffer[i] << Qt::hex << " " << Qt::endl;
//                                        stream << "2: " << QString::number(buffer[i], 16) << Qt::hex << " " << Qt::endl;
//                                        stream << "3: " << (void *)buffer[i] << Qt::hex << " " << Qt::endl;
//                                        QString valueInHex= QString("%1").arg(buffer[i] , 0, 16);
//                                        stream << "4: " << (void *)buffer[i] << Qt::hex << " " << Qt::endl;
                                        j++;
                                    }
                                    stream << Qt::endl;
                                } while (j < sizeof(SEQ_multi_buffer_s));
                            }

                            raw_data.push_back(0xF7);
                            midi_out.sendRawMessage(raw_data);

//                            QString s_file = QFileDialog::getOpenFileName( this->parent(),
//                                        tr("Select bin"),
//                                        tr("/"),
//                                        tr("Sysex ( *.bin);All Files ( * )"));

//                            QFile file;
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
                    break;
                }
                case MSG_PROJECT_SEND_PATTERN:

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

//void Worker::sendProjec(void)


//void Worker::updateUpdateFile(QString update_file_name)
//{

//}
