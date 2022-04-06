#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QThread"
#include "QTime"
#include "QFile"
#include "QFileDialog"
#include <QMessageBox>

#include "math.h"
#include "delay.h"
#include "midiworker.h"

#include "MIDI.h"
#include "Nibble.h"



const uint8_t goto_ble_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_BLE, 0xF7 };
const uint8_t goto_split_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_SPLIT, 0xF7 };

QByteArray sysex_cmd((char*)goto_ble_bootloader, 1024);
QByteArray sysex_file_buffer;


/*======================================================================*/
/*                         MAIN WINDOW                                  */
/*======================================================================*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    midiWorker = new MidiWorker(this);
    OxiDiscovery *discovery = midiWorker->GetDiscovery();

    connection_timer = new QTimer(this);

    // connect signal/slot
    connect(midiWorker, SIGNAL(ui_UpdateProgressBar(int)),
            this, SLOT(updateProgressBar(int)));

    connect(midiWorker, SIGNAL(ui_UpdateProjectProgressBar(int)),
            this, SLOT(updateMidiProgressBar(int)));

    connect(midiWorker, SIGNAL(ui_updateStatusLabel(QString)),
            this, SLOT(updateStatusLabel(QString)));

    connect(discovery, SIGNAL(ui_UpdateConnectionLabel(QString)),
            this, SLOT(updateConnectionLabel(QString)));

    connect(discovery, SIGNAL(ui_PortAlreadyInUse(void)),
            this, SLOT(uiPortAlreadyInUse()));

    connect(midiWorker, SIGNAL(ui_UpdateError(void)),
            this, SLOT(updateError(void)));

    connect(midiWorker, SIGNAL(ui_ConnectionError(void)),
            this, SLOT(connectionError(void)));

    connect(this, SIGNAL(updateWorkerDelayTime(int)),
            midiWorker, SLOT(ui_DelayTimeUpdated(int)));

    connect(connection_timer, SIGNAL(timeout()), discovery, SLOT(Discover()));

    connection_timer->start(500);

    ui->progressBar->setValue(0);
    ui->midiProgressBar->setValue(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::uiPortAlreadyInUse(){
    connection_timer->stop();
    QMessageBox::information(this, tr("Port Already in Use"),
    tr("Another process is already using the MIDI port.\n\nPlease ensure the port is not in use and restart OXI Desktop."));
}

void MainWindow::updateUiStatus(QString statusMessage){
    ui->connection_status_label->setText(statusMessage);
}

void MainWindow::updateProgressBar(int value)
{
    ui->progressBar->setValue(value);
}

void MainWindow::updateMidiProgressBar(int value)
{
    ui->midiProgressBar->setValue(value);
}

void MainWindow::updateStatusLabel(QString text)
{
    ui->process_status->setText(text);
}

void MainWindow::updateProjectStatusLabel(QString text)
{
//    ui->process_status->setText(text);
}

void MainWindow::updateConnectionLabel(QString text)
{
    ui->connection_status_label->setText(text);
}

void MainWindow::updateError(void)
{
    QMessageBox::warning(0, QString("Error"), QString("Update error"), QMessageBox::Ok);
}

void MainWindow::connectionError(void)
{
    QMessageBox::warning(0, QString("Error"), QString("Connection error"), QMessageBox::Ok);
}

// BLE
void MainWindow::on_gotoBLEBootloaderButton_clicked()
{
#if 1
    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_ONE_BLE_UPDATE;
        ui->process_status->setText("");
        midiWorker->LoadFile();
        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;

        // launch worker
        if (!midiWorker->isRunning()) {
            midiWorker->start();
        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
#endif
}

// SPLIT
void MainWindow::on_gotoSPLITBootloaderButton_clicked()
{
#if 1
    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_SPLIT_UPDATE;
        ui->process_status->setText("");
        midiWorker->LoadFile();
        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;

        // launch worker
        if (!midiWorker->isRunning()) {
            midiWorker->start();
        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
#endif
}

// OXI ONE
void MainWindow::on_gotoOXIBootloaderButton_clicked()
{
#if 1
    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_ONE_UPDATE;

        midiWorker->update_file_name_ = QFileDialog::getOpenFileName(
                    this,
                    tr("Select SYSEX"),
                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                    tr("Sysex ( *.syx);All Files ( * )"));

        qDebug() <<  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) << Qt::endl;
        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;
        ui->process_status->setText("UPDATING");
        qDebug() << "file selected" << Qt::endl;

        // launch worker
        if (!midiWorker->isRunning()) {
            midiWorker->start();
        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
#endif
}

void MainWindow::on_exitBootloaderButton_clicked()
{
#if 1
    if ( midiWorker->midi_out.isPortOpen()) {

        midiWorker->SendBootExit();

        if (midiWorker->isRunning()) {
            midiWorker->terminate();
            ui->progressBar->setValue(0);
        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
#endif
}


void MainWindow::on_stopButton_clicked()
{
    if (midiWorker->isRunning()) {
        midiWorker->terminate();
    }
    ui->progressBar->setValue(0);
}




void MainWindow::on_sendProjectButton_clicked()
{
    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->project_file_ =  QFileDialog::getOpenFileName(
                    this,
                    tr("Select project FILE"),
                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                    tr("Sysex ( *.bin);All Files ( * )"));


        qDebug() << midiWorker->project_file_ << Qt::endl;
        if (midiWorker->project_file_ == "" ) return;

        // launch worker
        midiWorker->SendProject();
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
}

void MainWindow::on_getProjectButton_clicked()
{
    midiWorker->GetProject();
}

void MainWindow::on_getPatternButton_clicked()
{
    midiWorker->GetPattern();
}

void MainWindow::on_seq_index_valueChanged(double arg1)
{
    midiWorker->seq_index_ = static_cast<int>(arg1) - 1;
}

void MainWindow::on_project_index_valueChanged(double arg1)
{
    midiWorker->project_index_ = static_cast<int>(arg1) - 1;
}

void MainWindow::on_pattern_index_valueChanged(double arg1)
{
    midiWorker->pattern_index_ = static_cast<int>(arg1) - 1;
}



void MainWindow::on_deleteProjectButton_clicked()
{
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_PROJECT);
    midiWorker->raw_data.push_back(MSG_PROJECT_DELETE_PROJECT);
    midiWorker->raw_data.push_back(midiWorker->project_index_);
    midiWorker->raw_data.push_back(0);
    midiWorker->raw_data.push_back(0xF7);

    midiWorker->SendRaw();
}


void MainWindow::on_deletePatternButton_clicked()
{
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_PROJECT);
    midiWorker->raw_data.push_back(MSG_PROJECT_DELETE_PATTERN);
    midiWorker->raw_data.push_back(midiWorker->project_index_);
    midiWorker->raw_data.push_back(midiWorker->seq_index_ * 16 + midiWorker->pattern_index_);
    midiWorker->raw_data.push_back(0xF7);

    midiWorker->SendRaw();
}

void MainWindow::on_getCalibDataButton_clicked()
{
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_SYSTEM);
    midiWorker->raw_data.push_back(MSG_SYSTEM_GET_CALIB_DATA);
    midiWorker->raw_data.push_back(0);
    midiWorker->raw_data.push_back(0);
    midiWorker->raw_data.push_back(0xF7);

    midiWorker->SendRaw();

    ui->midiProgressBar->setValue(0);
}


void MainWindow::on_sendCalibDataButton_clicked()
{
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_SYSTEM);
    midiWorker->raw_data.push_back(MSG_SYSTEM_SEND_CALIB_DATA);

    ui->process_status->setText("");

    QString calib_data_file = QFileDialog::getOpenFileName(
                this,
                tr("Select CALIBRATION FILE"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                tr("Sysex ( *.bin);All Files ( * )"));

    QFile calib_file;
    calib_file.setFileName(calib_data_file);
    if (!calib_file.open(QIODevice::ReadOnly))
        return;

    QByteArray sysex_file_buffer = calib_file.readAll();


    Nibblize(midiWorker->raw_data, (uint8_t*)sysex_file_buffer.data(), sysex_file_buffer.size());
    midiWorker->raw_data.push_back(0xF7);

     midiWorker->SendRaw();

    ui->midiProgressBar->setValue(0);
}

void MainWindow::on_eraseMemButton_clicked()
{
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_SYSTEM);
    midiWorker->raw_data.push_back(MSG_SYSTEM_MEM_RESET);
    midiWorker->raw_data.push_back(MSG_SYSTEM_MEM_RESET);
    midiWorker->raw_data.push_back(MSG_SYSTEM_MEM_RESET);
    midiWorker->raw_data.push_back(0xF7);

    midiWorker->SendRaw();

    ui->midiProgressBar->setValue(0);
}

