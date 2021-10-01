#include "worker.h"
#include "MIDI.h"
#include "string.h"
#include <QDebug>
#include <mainwindow.h>

//QString update_file_name_;

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
            goto EXIT;
        }

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
    emit ui_UpdateProgressBar(0);
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

void Worker::onMidiReceive(QMidiMessage* p_msg)
{
    if (p_msg->_status == MIDI_SYSEX)
    {
        //        char * p_SysExData = p_msg->getSysExData().data();
        if (memcmp(p_msg->getSysExData().data(), sysex_header, 6) == 0)
        {
            switch (p_msg->getSysExData()[6]) {
            case MSG_CAT_FW_UPDATE:
                switch (p_msg->getSysExData()[7]) {
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
