#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QThread"
#include "QTime"
#include "QFile"
#include "QFileDialog"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "math.h"
#include "delay.h"
#include "midiworker.h"

#include "MIDI.h"
#include "Nibble.h"
#include "miniz.h"


//const uint8_t goto_ble_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_BLE, 0xF7 };
//const uint8_t goto_split_bootloader[] = {0xF0, OXI_INSTRUMENTS_MIDI_ID OXI_ONE_ID MSG_CAT_FW_UPDATE, MSG_FW_UPDT_OXI_SPLIT, 0xF7 };

//QByteArray sysex_cmd((char*)goto_ble_bootloader, 1024);
//QByteArray sysex_file_buffer;


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

    connect(midiWorker, SIGNAL(SetFwVersion(QString)),
            this, SLOT(updateFwVersion(QString)));

    connect(this, SIGNAL(updateWorkerDelayTime(int)),
            midiWorker, SLOT(ui_DelayTimeUpdated(int)));

    connect(connection_timer, SIGNAL(timeout()), discovery, SLOT(Discover()));

    connection_timer->start(500);

    ui->progressBar->setValue(0);
    ui->midiProgressBar->setValue(0);

    _netManager = std::make_unique<QNetworkAccessManager>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::FileDialog(FileType type){
    QString filter = "";
    QString user_msg = "";
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    switch (type) {
    case FILE_SYSEX:
        user_msg = tr("Select SYSEX Update File");
        filter = "Sysex ( *" + QString(FileExtension[FILE_SYSEX]) + ")";
        break;
    case FILE_PROJECT:
        user_msg = tr("Select PROJECT File");
        filter = "Sysex ( *" + QString(FileExtension[FILE_PROJECT]) + ")";
        break;
    case FILE_PATTERN:
        user_msg = tr("Select PATTERN FILE");
        filter = "Sysex ( *" + QString(FileExtension[FILE_PATTERN]) + ")";
        break;
    case FILE_CALIBRATION:
        user_msg = tr("Select CALIBRATION File");
        filter = "Sysex ( *" + QString(FileExtension[FILE_CALIBRATION]) + ")";
        break;
    default:
        break;
    }

    QString ret = QFileDialog::getOpenFileName(
                        this,
                        user_msg,
                        path,
                        filter);

    return ret;
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
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(value);
    ui->progressBar->setMaximum(100);
}

void MainWindow::updateMidiProgressBar(int value)
{
    ui->midiProgressBar->setValue(value);
}

void MainWindow::updateStatusLabel(QString text)
{
    ui->process_status->setText(text);
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
    ui->process_status->setText("");

    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_ONE_BLE_UPDATE;
        ui->process_status->setText("");
        midiWorker->update_file_name_ = FileDialog(FILE_SYSEX);

        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;

        ui->process_status->setText("UPDATING");

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
    ui->process_status->setText("");

    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_SPLIT_UPDATE;
        ui->process_status->setText("");
        midiWorker->update_file_name_ = FileDialog(FILE_SYSEX);

        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;

        ui->process_status->setText("UPDATING");

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

int MainWindow::UncompressUpdateFile(const QString &filename, const QString &destDir)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    bool res = mz_zip_reader_init_file(&zip_archive, filename.toLatin1().constData(), 0);
    if (!res)
    {
        ui->process_status->setText(mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
        return EXIT_FAILURE;
    }

    uint count = mz_zip_reader_get_num_files(&zip_archive);
    if (count == 0)
    {
        mz_zip_reader_end(&zip_archive);
        return 0;
    }

    for (uint i = 0; i < count; ++i)
    {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
        {
            continue;
        }

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i))
        {
            // skip directories
            continue;
        }

        QString destfn = QFileInfo(QString(file_stat.m_filename)).fileName();
        QString destfilename = QString("%1/%2").arg(!destDir.isEmpty() ? destDir : ".", destfn);

        res = mz_zip_reader_extract_to_file(&zip_archive, i, destfilename.toLatin1().constData(), 0);
        if (!res)
        {
            ui->process_status->setText(mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
            return EXIT_FAILURE;
        }
    }

    mz_zip_reader_end(&zip_archive);

    return EXIT_SUCCESS;
}

QVersionNumber MainWindow::GetFrmVersion()
{
    // TODO: Get firmware version;
    return fw_version_;
}

void MainWindow::updateFwVersion(QString version)
{
    QVersionNumber temp = QVersionNumber::fromString(version);
    if (fw_version_ != temp) {
        fw_version_ = temp;

        qDebug() << fw_version_ << Qt::endl;
        DetectOXIOneAvailableUpdate();
    }
}

void MainWindow::DetectOXIOneAvailableUpdate()
{
    ui->gotoOXIBootloaderButton->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("https://gitlab.com/api/v4/projects/25470905/releases/"));
    // request.setUrl(QUrl("https://gitlab.com/api/v4/projects/25470905/releases/permalink/latest"));
    // request.setUrl(QUrl("https://gitlab.com/api/v4/projects/25470905/releases/3.2.3"));
    auto updateReply = _netManager->get(request);

    ui->process_status->setText("Detecting available update");

    connect(updateReply, &QNetworkReply::finished, [=]()
    {
        QApplication::restoreOverrideCursor();

        ui->gotoOXIBootloaderButton->setEnabled(true);

        ui->process_status->setText("");

        if (updateReply->error())
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

            ui->process_status->setText(QString("Error detecting available update: %1").arg(metaEnum.valueToKey(updateReply->error())));

            qDebug() << updateReply->errorString() << Qt::endl;
        }
        else
        {
            auto responseData = updateReply->readAll();
            auto jsonData = QJsonDocument::fromJson(responseData);
            // auto jsonObj = jsonData.object();
            QJsonArray dataObject = jsonData.array();
            auto jsonObj = dataObject.at(0);
            auto version = jsonObj["name"].toString();
            auto description = jsonObj["description"].toString();
            auto assets = jsonObj["assets"].toObject();
            auto links = assets["links"].toArray();

            QString updateFile;
            if (links.size() == 1)
            {
                auto linkObj = links[0].toObject();
                updateFile = linkObj["url"].toString();
            }

            auto latestVersion = QVersionNumber::fromString(version);

            if (!updateFile.isEmpty() &&
                    !latestVersion.isNull())
            {
                if (latestVersion > fw_version_)
                {
#if 0
                    auto choice = QMessageBox::question(this, "Update available", QString("Update to version %1 available to download. Do you wish to proceed?").arg(version));
                    if (choice == QMessageBox::StandardButton::Yes)
                    {
                        DownloadOXIOneAvailableUpdate(updateFile, version);
                    }
#endif
                    QMessageBox messageBox;
                    messageBox.setText(QString("Update to version %1 available to download. Do you wish to proceed?").arg(version));
                    QAbstractButton* helpButton = messageBox.addButton("What's new?", QMessageBox::HelpRole);
                    QAbstractButton* pButtonYes = messageBox.addButton("Yes", QMessageBox::YesRole);
                    messageBox.addButton("No", QMessageBox::NoRole);

                    // Add a "Help" button to the message box
                    helpButton->disconnect();

                    messageBox.setEscapeButton(QMessageBox::No);
                    messageBox.setWindowModality(Qt::ApplicationModal);
                    messageBox.setWindowFlags(Qt::Sheet);
                    messageBox.setModal(true);


                    connect(helpButton, &QAbstractButton::clicked, this, [=](){
                        // Handle the "Help" button action here
                        QString markdownText = description;
                        HelpDialog helpDialog(markdownText);
                        helpDialog.setWindowTitle("What's new?");
                        helpDialog.resize(450,600);
                        helpDialog.exec();
                        qDebug() << "HELP" << Qt::endl;
                    });

                    connect(pButtonYes, &QAbstractButton::clicked, this, [=](){
                        // Handle the "Help" button action here
                        DownloadOXIOneAvailableUpdate(updateFile, version);
                    });


                    messageBox.exec();

                }
                else
                {
                    ui->process_status->setText("Firmware version is up to date.");
                }
            }
            else
            {
                ui->process_status->setText("No update file found.");
            }
        }

        updateReply->deleteLater();
    });
}

void MainWindow::DownloadOXIOneAvailableUpdate(const QString& updateZipFileUrl, const QString& version)
{
    QNetworkRequest request;
    request.setUrl(QUrl(updateZipFileUrl));
    auto downloadUpdateReply = _netManager->get(request);

    // ui->progressBar->setTextVisible(true);
     ui->progressBar->setFormat("Downloading update");
     ui->progressBar->setValue(0);
    ui->process_status->setText("Downloading update");
    ui->gotoOXIBootloaderButton->setEnabled(false);

    connect(downloadUpdateReply, &QNetworkReply::downloadProgress, [=](auto bytesReceived, auto bytesTotal)
    {
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesReceived);
    });

    connect(downloadUpdateReply, &QNetworkReply::finished, [=]()
    {
        ui->process_status->setText("");
//        ui->progressBar->setTextVisible(false);
        ui->progressBar->setFormat("%p%");

//        ui->progressBar->reset();

        if (downloadUpdateReply->error())
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

            ui->process_status->setText(QString("Error downloading update file: %1").arg(metaEnum.valueToKey(downloadUpdateReply->error())));
        }
        else
        {
            _updateFileTempDir.reset(new QTemporaryDir());

            if (_updateFileTempDir->isValid())
            {
                QByteArray b = downloadUpdateReply->readAll();
                auto updateFilePath = QString("%1/OxiOneUpdate%2.syx").arg(_updateFileTempDir->path(), version);
                QSaveFile file(updateFilePath);
                if (file.open(QIODevice::WriteOnly))
                {
                    file.write(b);
                    if (file.commit())
                    {
                        DeployOXIOneUpdate(updateFilePath);

                    }
                    else
                    {
                        ui->process_status->setText("Unable to save update file to destination archive.");
                    }
                }
                else
                {
                    ui->process_status->setText("Unable to create destination archive for update file.");
                }
            }
            else
            {
                ui->process_status->setText("Unable to create save location for update file.");
            }
        }

        ui->gotoOXIBootloaderButton->setEnabled(true);
        downloadUpdateReply->deleteLater();
    });
}

void MainWindow::DeployOXIOneUpdate(const QString& updateFile)
{
#if 1
    ui->process_status->setText("");

    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->run_process_ = midiWorker->OXI_ONE_UPDATE;

        midiWorker->update_file_name_ = updateFile;

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
        QMessageBox::warning(this, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
#endif
}

// OXI ONE
void MainWindow::on_gotoOXIBootloaderButton_clicked()
{

#if 1
	ui->process_status->setText("");

	if ( midiWorker->midi_out.isPortOpen())
	{
		if (midiWorker->isRunning()) {
			midiWorker->terminate();
		}

		midiWorker->run_process_ = midiWorker->OXI_ONE_UPDATE;

		midiWorker->update_file_name_ = FileDialog(FILE_SYSEX);

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

        ui->process_status->setText("");
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
    ui->process_status->setText("");

    midiWorker->UpdateProjIdx(static_cast<int>(ui->project_index->value()) );

    if ( midiWorker->midi_out.isPortOpen())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }
        midiWorker->run_process_ = midiWorker->PROJECT_SEND;

        midiWorker->project_file_ =  FileDialog(FILE_PROJECT);

        qDebug() << midiWorker->project_file_ << Qt::endl;
        if (midiWorker->project_file_ == "" ) return;

        // launch worker
        if (!midiWorker->isRunning()) {
            midiWorker->start();
        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
}

void MainWindow::on_getProjectButton_clicked()
{
    ui->process_status->setText("");
    midiWorker->UpdateProjIdx(static_cast<int>(ui->project_index->value()) );
    midiWorker->GetProject();
}

void MainWindow::on_getPatternButton_clicked()
{
    midiWorker->GetPattern();
}

void MainWindow::on_project_index_valueChanged(double arg1)
{
    // midiWorker->project_index_ = static_cast<int>(arg1) - 1;
}



void MainWindow::on_deleteProjectButton_clicked()
{
    ui->process_status->setText("");
    midiWorker->UpdateProjIdx(static_cast<int>(ui->project_index->value()) );
    midiWorker->DeleteProject();
    ui->midiProgressBar->setValue(0);
}

#if 0
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
#endif

void MainWindow::on_getCalibDataButton_clicked()
{
    ui->process_status->setText("");
    midiWorker->GetCalibData();
    ui->midiProgressBar->setValue(0);
}


void MainWindow::on_sendCalibDataButton_clicked()
{
    // TODO move to MIDI Worker
    ui->process_status->setText("");
    midiWorker->raw_data.clear();
    midiWorker->raw_data.assign(sysex_header, &sysex_header[sizeof(sysex_header)]);
    midiWorker->raw_data.push_back(MSG_CAT_SYSTEM);
    midiWorker->raw_data.push_back(MSG_SYSTEM_SEND_CALIB_DATA);

    ui->process_status->setText("");

    QString calib_data_file = FileDialog(FILE_CALIBRATION);

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
    // TODO move to MIDI Worker

    ui->process_status->setText("");

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

void MainWindow::on_gotoOXIBootloaderButton_2_clicked()
{
    DetectOXIOneAvailableUpdate();
}

