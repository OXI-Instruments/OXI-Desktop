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
#include "midiworker.h"

#include "MIDI.h"
#include "Nibble.h"
#include "miniz.h"



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

    /* TIMER FOR OXI DISCOVERY
     *
     * */
    connection_timer = new QTimer(this);
    connect(connection_timer, SIGNAL(timeout()), discovery, SLOT(Discover()));
    connection_timer->start(500);


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

    connect(midiWorker, SIGNAL(ui_ProjectError(void)),
            this, SLOT(projectError(void)));

    connect(midiWorker, SIGNAL(ui_ConnectionError(void)),
            this, SLOT(connectionError(void)));

    connect(midiWorker, SIGNAL(ui_Success(void)),
            this, SLOT(processSuccess(void)));

    connect(midiWorker, SIGNAL(ui_lockUpdate(void)),
            this, SLOT(lockUpdateButtons(void)));

    connect(midiWorker, SIGNAL(SetFwVersion(QString)),
            this, SLOT(updateFwVersion(QString)));

    connect(midiWorker, SIGNAL(ResetFwVersion(void)),
            this, SLOT(resetFwVersion(void)));

    connect(this, SIGNAL(updateWorkerDelayTime(int)),
            midiWorker, SLOT(ui_DelayTimeUpdated(int)));

    connect(this, SIGNAL(workingDirectoryChanged(QString)),
            midiWorker, SLOT(setWorkerDirectory(QString)));

    ui->progressBar->setValue(0);
    ui->midiProgressBar->setValue(0);
    ui->process_status->setWordWrap(true);
    ui->process_status->setMaximumWidth(350);
    ui->process_status->setMaximumHeight(50);

    _netManager = std::make_unique<QNetworkAccessManager>();

    readSettings();
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

int MainWindow::CheckWorkerBusy() {
    if (midiWorker->isRunning()) {
        QMessageBox::warning(0, QString("Information"), QString("APP Busy, try later."), QMessageBox::Ok);
        return 1 ;
    }
    return 0;
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
    QMessageBox::warning(
                0,
                QString("Update error"),
                QString("Any error occurred during the update procedure.\n\r"
                        "If your device is unresponsive, do the following:\n\r"
                        "1. Hold the power button until the unit shuts down completely.\n\r"
                        "2. Turn the device in UPDATE MODE by holding Mute button and powering on.\n\r"
                        "3. Restart the firmware update procedure by either 'Get latest firmware' or 'Update from file'."),
                QMessageBox::Ok);

    unlockUpdateButtons();
}

void MainWindow::processSuccess(void)
{
    updateStatusLabel("SUCCESS!");
    unlockUpdateButtons();
}

void MainWindow::projectError(void)
{
    QMessageBox::warning(0, QString("Project error"), QString("Project error, please try again."), QMessageBox::Ok);
    unlockProjectButtons();
    updateStatusLabel("");
}

void MainWindow::connectionError(void)
{
    QMessageBox::warning(0, QString("Connection error"), QString("Connection error."), QMessageBox::Ok);
    updateStatusLabel("");
}

bool MainWindow::checkOXIconnected(void)
{
    if ( midiWorker->midi_out.isPortOpen()) {
        return true;
    } else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
        return false;
    }
}

// BLE
void MainWindow::on_gotoBLEBootloaderButton_clicked()
{
#if 1
    ui->process_status->setText("");

    if ( checkOXIconnected())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->SetState(midiWorker->WORKER_FW_UPDATE_BLE);
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
#endif
}

// SPLIT
void MainWindow::on_gotoSPLITBootloaderButton_clicked()
{
#if 1
    ui->process_status->setText("");

    if ( checkOXIconnected())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->SetState(midiWorker->WORKER_FW_UPDATE_SPLIT);
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
    return fw_version_;
}

int MainWindow::SetWorkingDirectory() {
    QMessageBox::information(
        0,
        QString("Working folder"),
        QString("Select your folder for the OXI One projects and data..."),
        QMessageBox::Ok);

    bool already_created = 1;

    QString dialogDir;
    if (workingDirectory_.isEmpty()) {
        dialogDir = QDir::homePath();
    } else {
        dialogDir = workingDirectory_;
    }

    QString workingDirectoryFromDialog;

    workingDirectoryFromDialog = QFileDialog::getExistingDirectory(
        this,
        tr("Select your folder for the OXI One projects and data."),
        dialogDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!workingDirectoryFromDialog.isNull()) {
        workingDirectory_ = workingDirectoryFromDialog;
        oxi_path_ = workingDirectory_ + "/OXI_Files";

        QDir system_dir;
        if (!system_dir.exists(oxi_path_)) {
            already_created = 0;
            system_dir.mkdir(oxi_path_);
        }

        QDir dir = QDir::current(); // current directory
        QString absolutePath = dir.absoluteFilePath(oxi_path_);
        QString message = QString("Selected folder:\n%1").arg(absolutePath);
        ui->process_status->setText(message);

        // Save directory
        writeSettings();
        emit workingDirectoryChanged(oxi_path_);
    }
    // que pasa si el usuario cancela??
    else {

    }

    return already_created;
}


void MainWindow::resetFwVersion(void)
{
    fw_version_ = QVersionNumber::fromString("");
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
   lockUpdateButtons();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl("https://gitlab.com/api/v4/projects/25470905/releases/"));
    auto updateReply = _netManager->get(request);

    ui->process_status->setText("Detecting available update");

    connect(updateReply, &QNetworkReply::finished, [=]()
    {
        QApplication::restoreOverrideCursor();
        unlockUpdateButtons();

        ui->process_status->setText("");

        if (updateReply->error())
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

            ui->process_status->setText(QString("Error detecting available update: %1").arg(metaEnum.valueToKey(updateReply->error())));

            qDebug() << updateReply->errorString() << Qt::endl;
            qDebug() << QSslSocket::supportsSsl();
        }
        else
        {
            auto responseData = updateReply->readAll();
            auto jsonData = QJsonDocument::fromJson(responseData);
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
    ui->updateFromFileButton->setEnabled(false);

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

        ui->updateFromFileButton->setEnabled(true);
        downloadUpdateReply->deleteLater();
    });
}

void MainWindow::DeployOXIOneUpdate(const QString& updateFile)
{
#if 1
    ui->process_status->setText("");

    if (CheckWorkerBusy()) return;
    if ( checkOXIconnected())
    {
        if (midiWorker->isRunning()) {
            midiWorker->quit();
        }
        }

        midiWorker->SetState(midiWorker->WORKER_FW_UPDATE_OXI_ONE);

        midiWorker->update_file_name_ = updateFile;

        qDebug() <<  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) << Qt::endl;
        qDebug() << midiWorker->update_file_name_ << Qt::endl;
        if (midiWorker->update_file_name_ == "" ) return;

        ui->process_status->setText("UPDATING");

        qDebug() << "file selected" << Qt::endl;

        // launch worker
        midiWorker->start();
    }
#endif


// OXI ONE
void MainWindow::on_updateFromFileButton_clicked()
{

#if 1
    ui->process_status->setText("");

    if (CheckWorkerBusy()) return;

    if ( checkOXIconnected())
    {
        if (midiWorker->isRunning()) {
            midiWorker->terminate();
        }

        midiWorker->SetState(midiWorker->WORKER_FW_UPDATE_OXI_ONE);

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
#endif


}

#if 0
void MainWindow::on_exitBootloaderButton_clicked()
{
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
}


void MainWindow::on_stopButton_clicked()
{
    if (midiWorker->isRunning()) {
        midiWorker->terminate();
    }
    ui->progressBar->setValue(0);
}
#endif


void MainWindow::on_sendProjectButton_clicked()
{
    if (CheckWorkerBusy()) return;

    ui->process_status->setText("");

    int proj_idx = static_cast<int>(ui->project_index->value());
    midiWorker->UpdateProjIdx( proj_idx );

    if ( checkOXIconnected())
    {
        QMessageBox::StandardButton reply;
        QString question = QString("This will overwrite OXI One's project %1.\nAre you sure you want to proceed?").arg(proj_idx);
        reply = QMessageBox::question(nullptr, "Confirmation", question, QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {

            midiWorker->SetState(midiWorker->WORKER_SEND_PROJECT);

            midiWorker->project_file_ =  FileDialog(FILE_PROJECT);

            qDebug() << midiWorker->project_file_ << Qt::endl;
            if (midiWorker->project_file_ == "" ) return;

            // launch worker
            midiWorker->start();

            QMessageBox::StandardButton cancelReply = QMessageBox::Yes;
            QString questionCancel = QString("Would you like to cancel the process?" );

            //casting to remove the NO option. Dunno why states question is deprecated while works previously
            cancelReply = static_cast<QMessageBox::StandardButton>(QMessageBox::question(nullptr, "Confirmation", questionCancel, QMessageBox::Yes));
            if (cancelReply == QMessageBox::Yes) {

                midiWorker->SetState(midiWorker->WORKER_CANCELLING);
                ui->midiProgressBar->setValue(0);

            }


        } else {
            // User clicked No or closed the dialog
            // Do something else...
        }
    }
}
// GET SINGLE PROJECT
void MainWindow::on_getProjectButton_clicked()
{
    if (CheckWorkerBusy()) return;

    ui->process_status->setText("");

    if (checkOXIconnected())
    {
        bool folder_existed = 0;
        if (GetWorkingDirectory().isEmpty()){
            folder_existed = SetWorkingDirectory();

            if (GetWorkingDirectory().isEmpty())
                return;
        }
        else
            folder_existed = 1;



        QMessageBox::StandardButton reply = QMessageBox::Yes;
        if (folder_existed) {
            QString question = QString("This will overwrite the existing Project of your OXI One computer folder.\nContinue?" );
            reply = QMessageBox::question(nullptr, "Confirmation", question, QMessageBox::Yes|QMessageBox::No);
        }

        if (reply == QMessageBox::Yes) {
            ui->process_status->setText("");
            midiWorker->UpdateProjIdx(static_cast<int>(ui->project_index->value()) );

            midiWorker->SetState(midiWorker->WORKER_GET_PROJECT);
            midiWorker->start();
            // midiWorker->GetSingleProject();
            midiWorker->runGetProject();



            //QMessageBox::StandardButton cancelReply = QMessageBox::Yes;
            QMessageBox msgBox;
            msgBox.setText("Would you like to cancel the process?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            msgBox.setEscapeButton(QMessageBox::Cancel);

            // Hide the Cancel button
            QAbstractButton* cancelButton = msgBox.button(QMessageBox::Cancel);
            if (cancelButton)
                cancelButton->setVisible(false);

            QMessageBox::StandardButton cancelReply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

            if (cancelReply == QMessageBox::Yes) {

                midiWorker->SetState(midiWorker->WORKER_CANCELLING);

                qDebug() << "------------USER CLICKED ON YES TO CANCEL------------";
                //the one below might need to be implemented somewhere else?
                ui->midiProgressBar->setValue(0);

            }
        }
    }
}

// GET ALL PROJECTS
void MainWindow::on_getAllProjectButton_clicked()
{

    if (CheckWorkerBusy()) return;
    ui->process_status->setText("");

    if ( checkOXIconnected())
    {
        bool folder_existed = 0;
        //bool isGetProjectRunning = 0;  //This would not be required if cancel button is on top

        if (GetWorkingDirectory().isEmpty()){
            folder_existed = SetWorkingDirectory();

            if (GetWorkingDirectory().isEmpty())
                return;
        }

        else
            folder_existed = 1;


        //if(isGetProjectRunning == 0){
            QMessageBox::StandardButton reply = QMessageBox::Yes;
            if (folder_existed) {
                QString question = QString("This will overwrite ALL the existing Projects of your OXI One computer folder.\nContinue?" );
                reply = QMessageBox::question(nullptr, "Confirmation", question, QMessageBox::Yes|QMessageBox::No);
            }

            if (reply == QMessageBox::Yes) {
                ui->process_status->setText("");
                //midiWorker->UpdateProjIdx(static_cast<int>(ui->project_index->value()) );
                midiWorker->GetAllProjects();

                //isGetProjectRunning = 1;

                QMessageBox::StandardButton cancelReply = QMessageBox::Yes;
                QString questionCancel = QString("Would you like to cancel the process?" );

                //casting to remove the NO option. Dunno why states question is deprecated while works previously
                cancelReply = static_cast<QMessageBox::StandardButton>(QMessageBox::question(nullptr, "Confirmation", questionCancel, QMessageBox::Yes));
                if (cancelReply == QMessageBox::Yes) {

                    //pggoxi: revise
                    midiWorker->SetState(midiWorker->WORKER_CANCELLING);
                    //the one below might need to be implemented somewhere else?
                    ui->midiProgressBar->setValue(0);

                }


            }



            /*
            if (isGetProjectRunning == 1){

                QMessageBox::StandardButton cancelReply = QMessageBox::Yes;
                QString questionCancel = QString("Would you like to cancel the process?" );

                //casting to remove the NO option. Dunno why states question is deprecated while works previously
                cancelReply = static_cast<QMessageBox::StandardButton>(QMessageBox::question(nullptr, "Confirmation", questionCancel, QMessageBox::Yes));
                if (cancelReply == QMessageBox::Yes) {

                    //pggoxi: HERE must end the Thread
                    //the pop up should vanish when finished
                    midiWorker -> quit(); //quit does not stop the thread. should be implemented

                }

            */
            //}

        //}

    }
}

#if 0
void MainWindow::on_getPatternButton_clicked()
{
    if (DefaultWorkingDirectory())
        SetWorkingDirectory();

    midiWorker->GetPattern();
}
#endif

void MainWindow::on_project_index_valueChanged(double arg1)
{
    // midiWorker->project_index_ = static_cast<int>(arg1) - 1;
}



void MainWindow::on_deleteProjectButton_clicked()
{
    if (CheckWorkerBusy()) return;

    if ( midiWorker->midi_out.isPortOpen())
    {
        int proj_idx = static_cast<int>(ui->project_index->value());
        midiWorker->UpdateProjIdx( proj_idx );

        QMessageBox::StandardButton reply;
        QString question = QString("This will delete OXI One's Project %1.\nAre you sure you want to proceed?").arg(proj_idx);

        reply = QMessageBox::question(nullptr, "Confirmation", question, QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {

            midiWorker->DeleteProject();
            ui->midiProgressBar->setValue(100);

        }
    }
    else {
        QMessageBox::warning(0, QString("Information"), QString("Connect your OXI One"), QMessageBox::Ok);
    }
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
    if (CheckWorkerBusy()) return;

    if (GetWorkingDirectory().isEmpty())
        SetWorkingDirectory();

    ui->process_status->setText("");
    midiWorker->GetCalibData();
    ui->midiProgressBar->setValue(0);
}


void MainWindow::on_sendCalibDataButton_clicked()
{
    if (CheckWorkerBusy()) return;
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

void MainWindow::on_getLatestFW_Button_clicked()
{
    DetectOXIOneAvailableUpdate();
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index > 0 && GetWorkingDirectory().isEmpty()) {
        SetWorkingDirectory();
    } else {
        QDir dir;
        QString absolutePath = dir.absoluteFilePath(oxi_path_);
        QString message = QString("Selected folder:\n%1").arg(absolutePath);
        ui->process_status->setText(message);
    }
}


void MainWindow::on_setWorkingFolderButton_clicked()
{
    SetWorkingDirectory();
}



