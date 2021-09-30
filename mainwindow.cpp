#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QThread"
#include "QTime"
#include "QFile"
#include "QFileDialog"

#include "math.h"
#include "worker.h"

#include "SYSEX_PROJ.h"
#include "MIDI.h"

//uint8_t cmd[128]; // = {0xF0, 0x00, 0x22, 0x03, 0x00, 0x02, 0x7F, 0x00, 0xF7 };
uint8_t goto_ble_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_BLE, 0xF7 };
uint8_t goto_oxi_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_ONE, 0xF7 };
uint8_t cmd2[] =            {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_STARTED, 0xF7 };
uint8_t exit_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_EXIT, 0xF7 };

QByteArray sysex_cmd((char*)goto_ble_bootloader, 1024);
//QByteArray sysex_cmd((char*)goto_bootloader, sizeof(goto_bootloader));
QByteArray sysex_file_buffer;

std::vector<unsigned char> raw_data2;





SYSEX_ProjInfo_s proj_info = {
    .project_num = 0,
    .project_name = {0}, //"Project",
    .tempo = 1000,
    .pattern_name = {{{0}}},
    .song_name = {{0}},
    .checksum = 0,
};

void delay(int m_sec)
{
    QTime dieTime= QTime::currentTime().addMSecs(m_sec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


/*======================================================================*/
/*                         MAIN WINDOW                                  */
/*======================================================================*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    mWorker = new Worker(this);
    connection_timer = new QTimer(this);

    // connect signal/slot
    connect(mWorker, SIGNAL(ui_UpdateProgressBar(int)),
            this, SLOT(updateProgressBar(int)));
    connect(this, SIGNAL(updateWorkerDelayTime(int)),
            mWorker, SLOT(ui_DelayTimeUpdated(int)));

    connect(this, SIGNAL(WorkerRefreshDevices(void)),
            mWorker, SLOT(WorkerRefreshDevices(void)));

    //    connect(this, SIGNAL(WorkerRefreshDevices(void)),
    //            mWorker, SLOT(updateUpdateFile(QString)));

    connect(connection_timer, SIGNAL(timeout()), this, SLOT(ConnectionCheck()));

    //    connect(&mWorker->midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),mWorker, SLOT(onMidiReceive(QMidiMessage*)));

    ui->progressBar->setValue(0);

    on_refreshButton_clicked();

    // on_devicesComboBox_activated(0);
}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::ConnectionCheck(void)
{

}

void MainWindow::on_lineEdit_returnPressed()
{
#if 0
    midi_out.sendSysEx(sysex_cmd);
#endif
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
#if 0
    sysex_cmd.setRawData(arg1.toUtf8(), arg1.size());
#endif
}


void MainWindow::on_devicesComboBox_currentIndexChanged(int index)
{
#if 0
    if ((unsigned int)index < mWorker->midi_out.getPortCount())
    {
        mWorker->midi_out.closePort();
        mWorker->midi_in.closePort();

        mWorker->midi_out.openPort(index);
        mWorker->midi_in.openPort(index);

        qDebug() << "Connected to " <<  mWorker->midi_out.getPorts()[index] << Qt::endl;
    }

#endif
}


void MainWindow::updateProgressBar(int value)
{
    ui->progressBar->setValue(value);
}



void MainWindow::on_refreshButton_clicked()
{
    //    emit WorkerRefreshDevices();

    QStringList midi_list =  mWorker->midi_out.getPorts();

    ui->devicesComboBox->clear();

    for (int i = 0; i != midi_list.size(); ++i)
    {
        ui->devicesComboBox->addItem(midi_list[i], i);
    }

}

void MainWindow::on_gotoBLEBootloaderButton_clicked()
{
#if 1
    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        mWorker->raw_data.assign(&goto_ble_bootloader[0], &goto_ble_bootloader[sizeof(goto_ble_bootloader)]);
        mWorker->midi_out.sendRawMessage( mWorker->raw_data);

    }
#endif
}

void MainWindow::on_gotoOXIBootloaderButton_clicked()
{
#if 1
    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        mWorker->raw_data.assign(&goto_oxi_bootloader[0], &goto_oxi_bootloader[sizeof(goto_oxi_bootloader)]);
        mWorker->midi_out.sendRawMessage( mWorker->raw_data);

    }
#endif
}

void MainWindow::on_exitBootloaderButton_clicked()
{
#if 1
    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        mWorker->raw_data.assign (&exit_bootloader[0], &exit_bootloader[sizeof(exit_bootloader)]);
        mWorker->midi_out.sendRawMessage( mWorker->raw_data);

    }
#endif
}

void MainWindow::on_loadFileButton_clicked()
{
    mWorker->Stop = false;
    mWorker->update_file_name_ = QFileDialog::getOpenFileName(
                this,
                tr("Select SYSEX"),
                tr("/Users/ManuelVr/Desktop"),
                tr("Sysex ( *.syx);All Files ( * )"));

    mWorker->start();
}

void MainWindow::on_stopButton_clicked()
{
    if (mWorker->isRunning()) {
        mWorker->terminate();
    }
    ui->progressBar->setValue(0);
}


void MainWindow::on_devicesComboBox_activated(int index)
{
    if ((unsigned int)index < mWorker->midi_out.getPortCount())
    {
        mWorker->midi_out.closePort();

        QStringList midi_out_ports = mWorker->midi_out.getPorts();
        QString midi_out_q_string = midi_out_ports[index];
        QByteArray midi_out_port = midi_out_ports[index].toLocal8Bit();
        const char *midi_out_name = midi_out_port.data();

        if (qstrncmp (midi_out_name, "OXI", 3) == 0) {
            mWorker->midi_out.openPort(index, midi_out_q_string);
            qDebug() << "Connected to " <<  mWorker->midi_out.getPorts()[index] << Qt::endl;

            if ((unsigned int)index < mWorker->midi_in.getPortCount())
            {

                QStringList midi_in_ports = mWorker->midi_in.getPorts();
                QString midi_in_q_string = midi_in_ports[index];
                QByteArray midi_in_port = midi_in_ports[index].toLocal8Bit();
                const char *midi_in_name = midi_in_port.data();

                mWorker->midi_in.closePort();

                if (qstrncmp (midi_in_name, "OXI", 3) == 0) {
                    mWorker->midi_in.openPort(midi_in_q_string);
                    qDebug() << "Connected to " <<  mWorker->midi_in.getPorts()[index] << Qt::endl;
                } else {
                    qDebug() << "OXI ONE not recognized";
                }
            }
        } else {
            qDebug() << "OXI ONE not recognized";
        }


    }

}



void MainWindow::on_sendProjectButton_clicked()
{
    std::snprintf(proj_info.song_name[0].name, PROJ_NAME_LEN, "%s", ui->lineEdit_projectName->text().toLocal8Bit().constData());

//    proj_info.song_name[0].name << ui->lineEdit_projectName->text().toLocal8Bit().constData();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            // sprintf(proj_info.pattern_name[i][j].name, "Pat %d Seq%d", j, i);
        }
    }
    for (int j = 0; j < 16; j++) {
        // sprintf(proj_info.song_name[j].name, "Song %d", j);
    }
    strcpy(proj_info.song_name[0].name, "Juntitos");
    strcpy(proj_info.song_name[SONG_NUM - 1].name, "Muy Top");
    sprintf(proj_info.pattern_name[0][0].name, "Main Lead");

    for (uint16_t i = 0; i < (sizeof(SYSEX_ProjInfo_s) - 4); i++)
    {
        proj_info.checksum += ((uint8_t*)&proj_info)[i];
    }

    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        std::vector<unsigned char>* p_data = &mWorker->raw_data;
        uint8_t * p_project_array = (uint8_t*)&proj_info;

        p_data->assign(&sysex_header[0], &sysex_header[sizeof(sysex_header)]);
        p_data->push_back(MSG_CAT_PROJECT);
        p_data->push_back(MSG_PROJECT_SET_INFO);

        uint8_t temp_buff[sizeof(SYSEX_ProjInfo_s) * 2 + 1];

        for (uint16_t i = 0; i < sizeof(SYSEX_ProjInfo_s); i++)
        {
            temp_buff[i * 2] = p_project_array[i] >> 4;
            temp_buff[i * 2 + 1] = p_project_array[i] & 0xf;
        }
        temp_buff[sizeof(SYSEX_ProjInfo_s) * 2] = 0xF7;

        p_data->insert(p_data->end(), temp_buff, temp_buff + sizeof(SYSEX_ProjInfo_s) * 2 + 1);
        mWorker->midi_out.sendRawMessage(*p_data);

    }
}
