#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "oxidiscovery.h"
#include <QMainWindow>
#include "midiworker.h"

#include "FileTypes.h"
#include "ui_mainwindow.h"


#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

class QNetworkAccessManager;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MidiWorker *midiWorker;
    OxiDiscovery *discovery;
    QTimer *connection_timer;

    //    QMidiIn midi_in;
    //    QMidiOut midi_out;
    //    std::vector<unsigned char> raw_data;

private slots:

    void updateProgressBar(int value);
    void updateMidiProgressBar(int value);
    void updateStatusLabel(QString text);
    void updateConnectionLabel(QString text);
    void updateFwVersion(QString version);
    void resetFwVersion(void);
    void updateError(void);
    void processSuccess(void);
    void connectionError(void);
    void projectError(void);
    void uiPortAlreadyInUse(void);

    void lockUpdateButtons(void)
    {
        ui->gotoOXIBootloaderButton_2->setEnabled(false);
        ui->gotoOXIBootloaderButton->setEnabled(false);
        ui->gotoBLEBootloaderButton->setEnabled(false);
    }

    void unlockUpdateButtons(void)
    {
        ui->gotoOXIBootloaderButton_2->setEnabled(true);
        ui->gotoOXIBootloaderButton->setEnabled(true);
        ui->gotoBLEBootloaderButton->setEnabled(true);
    }

    void lockProjectButtons(void)
    {
        ui->getProjectButton->setEnabled(false);
        ui->sendProjectButton->setEnabled(false);
        ui->deleteProjectButton->setEnabled(false);
    }

    void unlockProjectButtons(void)
    {
        ui->getProjectButton->setEnabled(true);
        ui->sendProjectButton->setEnabled(true);
        ui->deleteProjectButton->setEnabled(true);
    }

    void on_gotoBLEBootloaderButton_clicked();
    void on_gotoSPLITBootloaderButton_clicked();
    void on_gotoOXIBootloaderButton_clicked();
#if 0
    void on_exitBootloaderButton_clicked();

    void on_stopButton_clicked();

    void on_getPatternButton_clicked();
#endif
    void on_sendProjectButton_clicked();

    void on_getProjectButton_clicked();

    void on_project_index_valueChanged(double arg1);


    void on_deleteProjectButton_clicked();

    void on_getCalibDataButton_clicked();

    void on_sendCalibDataButton_clicked();

    void on_eraseMemButton_clicked();

    QString FileDialog(FileType file);

    void on_gotoOXIBootloaderButton_2_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_setWorkingFolderButton_clicked();

    void on_exportToMIDI_clicked();

    void on_importFromMIDI_clicked();

signals:
    void updateWorkerDelayTime(int);
    void WorkerUpdateFile(QString);
    void workingDirectoryChanged(QString);

private:
    Ui::MainWindow *ui;
    void updateUiStatus(QString statusMessage);

private:
    int UncompressUpdateFile(const QString &filename, const QString &destDir);
    void DetectOXIOneAvailableUpdate();
    void DownloadOXIOneAvailableUpdate(const QString& updateZipFileUrl, const QString& version);
    void DeployOXIOneUpdate(const QString& updateFile);
    QVersionNumber GetFrmVersion();

    bool DefaultWorkingDirectory() {
        QString temp = workingDirectory_ + "/OXI_Files";
        QDir temp_dir;
        bool created = false;
        if (temp_dir.exists(temp)) {
            created = true;
        }
        return workingDirectory_ == oxiFilesDir_ || !created;
    }

    void SetWorkingDirectory() {
        QMessageBox::information(
                    0,
                    QString("Working folder"),
                    QString("Select your folder for the OXI One projects and data..."),
                    QMessageBox::Ok);

        workingDirectory_ = QFileDialog::getExistingDirectory(
                    this,
                    tr("Select your folder for the OXI One projects and data."),
                    QDir::homePath(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        oxi_path_ = workingDirectory_ + "/OXI_Files";



        QDir system_dir;
        if (!system_dir.exists(oxi_path_)) {

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

    void writeSettings()
    {
        QSettings settings("OXI Instruments", "OXI ONE DESKTOP");

        settings.setValue("WorkingDirectory", workingDirectory_);
    }

    void readSettings()
    {
        QSettings settings("OXI Instruments", "OXI ONE DESKTOP");
        workingDirectory_ = settings.value("WorkingDirectory", oxiFilesDir_).toString();
        qDebug() << "Stored working directory: " + workingDirectory_ << Qt::endl;

       // if (DefaultWorkingDirectory())
         //   SetWorkingDirectory();
    }

private:
    std::unique_ptr<QNetworkAccessManager> _netManager;
    std::unique_ptr<QTemporaryDir> _updateFileTempDir;

    QVersionNumber fw_version_;

    QString workingDirectory_ = "";
    QString oxi_path_ = "";

    const QString oxiFilesDir_ =  QCoreApplication::applicationDirPath() + "/../../.." ;
};




// Help/what's new window with markdown support
class HelpDialog : public QDialog
{
public:
    HelpDialog(const QString& markdownText, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        QTextBrowser *textBrowser = new QTextBrowser(this);
        textBrowser->setMarkdown(markdownText);
        layout->addWidget(textBrowser);
    }
};


#endif // MAINWINDOW_H
