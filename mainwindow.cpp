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
#include <nlohmann/json.hpp>

//uint8_t cmd[128]; // = {0xF0, 0x00, 0x22, 0x03, 0x00, 0x02, 0x7F, 0x00, 0xF7 };
uint8_t goto_ble_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_BLE, 0xF7 };
uint8_t goto_split_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_SPLIT, 0xF7 };
uint8_t goto_oxi_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_ONE, 0xF7 };
uint8_t cmd2[] =            {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_STARTED, 0xF7 };
uint8_t exit_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_EXIT, 0xF7 };

QByteArray sysex_cmd((char*)goto_ble_bootloader, 1024);
//QByteArray sysex_cmd((char*)goto_bootloader, sizeof(goto_bootloader));
QByteArray sysex_file_buffer;

std::vector<unsigned char> raw_data2;

using json = nlohmann::json;



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

    connection_timer->start(1000);

    //    connect(&mWorker->midi_in, SIGNAL(midiMessageReceived(QMidiMessage*)),mWorker, SLOT(onMidiReceive(QMidiMessage*)));

    ui->progressBar->setValue(0);

}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateProgressBar(int value)
{
    ui->progressBar->setValue(value);
}



void MainWindow::ConnectionCheck(void)
{
    //    emit WorkerRefreshDevices();

    QStringList midi_out_list =  mWorker->midi_out.getPorts();
    QStringList midi_in_list =  mWorker->midi_in.getPorts();
    QString oxi = QString("OXI");
#if defined(__LINUX_ALSA__)
    QString port_in_str = QString(":0");
    QString port_out_str = QString(":0");
#elif defined(__WINDOWS_MM__)
    QString port_in_str = QString("0");
    QString port_out_str = QString("1");
#elif defined(__MACOSX_CORE__)
    QString port_in_str = QString("1");
    QString port_out_str = QString("1");
#endif
    QString fw_update = QString("FW Update");

    bool found = false;
    // Port already open, check changes in ports
    if (mWorker->midi_out.isPortOpen()) {
        for (int i = 0; i != midi_out_list.size(); ++i)
        {
            if (mWorker->port_out_string == midi_out_list[i]) {
                found = true;
                break;
            }
        }
    }

    if (!found && mWorker->port_out_string != "")
    {
        mWorker->port_out_string = "";
        ui->connection_status_label->setText("CONNECT YOUR OXI ONE");
    }

    // Port not open or has changed
    if ((!mWorker->midi_out.isPortOpen()) || (found == false)) {
        for (int i = 0; i != midi_out_list.size(); ++i)
        {
            if ((midi_out_list[i].contains(port_out_str) || (midi_out_list[i].contains(fw_update))) &&
                    midi_out_list[i].contains(oxi))
            {
                mWorker->midi_out.closePort();

                try {
                    mWorker->midi_out.openPort(i);
                }
                catch ( RtMidiError &error ) {
                    error.printMessage();
                    return;
                }

                mWorker->port_out_string = midi_out_list[i];
                ui->connection_status_label->setText("OXI ONE CONNECTED");

                qDebug() << "Connected to " <<  midi_out_list[i] << Qt::endl;
                break;
            }
        }
    }


    found = false;
    // Port already open, check changes in ports
    if (mWorker->midi_in.isPortOpen()) {
        for (int i = 0; i != midi_in_list.size(); ++i)
        {
            if (mWorker->port_in_string == midi_in_list[i]) {
                found = true;
                break;
            }
        }
    }

    if (!found && mWorker->port_in_string != "") mWorker->port_in_string = "";

    // Port not open or has changed
    if ((!mWorker->midi_in.isPortOpen()) || (!found)) {
        for (int i = 0; i != midi_in_list.size(); ++i)
        {
            if ((midi_in_list[i].contains(port_in_str) || (midi_in_list[i].contains(fw_update))) &&
                    midi_in_list[i].contains(oxi))
            {
                mWorker->midi_in.closePort();

                try {
                    mWorker->midi_in.openPort(i);
                }
                catch ( RtMidiError &error ) {
                    error.printMessage();
                    return;
                }


                mWorker->port_in_string = midi_in_list[i];

                qDebug() << "Connected to " <<  midi_in_list[i] << Qt::endl;
                break;
            }
        }
    }

}

void MainWindow::on_gotoBLEBootloaderButton_clicked()
{
#if 1
    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        mWorker->raw_data.assign(&goto_ble_bootloader[0], &goto_ble_bootloader[sizeof(goto_ble_bootloader)]);
        mWorker->midi_out.sendRawMessage( mWorker->raw_data);
        mWorker->wait_for_ack = true;
    }
#endif
}

void MainWindow::on_gotoSPLITBootloaderButton_clicked()
{
#if 1
    if ( mWorker->midi_out.isPortOpen()) {

        //        std::vector<unsigned char>
        mWorker->raw_data.assign(&goto_split_bootloader[0], &goto_split_bootloader[sizeof(goto_split_bootloader)]);
        mWorker->midi_out.sendRawMessage( mWorker->raw_data);
        mWorker->wait_for_ack = false;
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
        mWorker->wait_for_ack = true;
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


void MainWindow::on_sendProjectButton_clicked()
{
    std::snprintf(proj_info.song_name[0].name, PROJ_NAME_LEN, "%s", ui->lineEdit_projectName->text().toLocal8Bit().constData());

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            // sprintf(proj_info.pattern_name[i][j].name, "Pat %d Seq%d", j, i);
        }
    }
    for (int j = 0; j < 16; j++) {
        // sprintf(proj_info.song_name[j].name, "Song %d", j);
    }

    const QString fileName = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../../" + "projects.json");
    qDebug() <<fileName;
    QFile projectsFile(fileName);
    if (projectsFile.open(QIODevice::ReadOnly)) {
     qDebug()<< "The file "<< fileName<< " has been opened successfully";
    } else {
      qDebug()<< "Failed to open file "<< fileName;
    }

    QByteArray projectLines = projectsFile.readAll();

    //    proj_info.song_name[0].name << ui->lineEdit_projectName->text().toLocal8Bit().constData();

    auto projs = json::parse(projectLines.toStdString());
    auto proj0 = projs.at("projects")[0];
    auto proj0Songs = proj0.at("songs");

    auto proj0Name = proj0Songs[0].at("song_name").dump();
    strcpy(proj_info.song_name[0].name, proj0Name.c_str());
    auto projLastName = proj0Songs[SONG_NUM - 1].at("song_name").dump();
    strcpy(proj_info.song_name[SONG_NUM - 1].name, projLastName.c_str());

    auto proj0Seqs = proj0.at("sequences");
    auto proj0Seqs0Patterns = proj0Seqs[0].at("patterns");
    auto patternName = proj0Seqs0Patterns[0].at("pattern_name").dump();
    strcpy(proj_info.pattern_name[0][0].name, patternName.c_str());

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
