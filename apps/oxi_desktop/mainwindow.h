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

    bool checkOXIconnected(void);

    void lockUpdateButtons(void)
    {
        ui->getLatestFW_Button->setEnabled(false);
        ui->updateFromFileButton->setEnabled(false);
        ui->gotoBLEBootloaderButton->setEnabled(false);
    }

    void unlockUpdateButtons(void)
    {
        ui->getLatestFW_Button->setEnabled(true);
        ui->updateFromFileButton->setEnabled(true);
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
    void on_updateFromFileButton_clicked();
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

    void on_getLatestFW_Button_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_setWorkingFolderButton_clicked();

    void on_getAllProjectButton_clicked();

    //bool onWorkFinished(bool flag);

    void cancelProgressBox();

signals:
    void updateWorkerDelayTime(int);
    void WorkerUpdateFile(QString);
    void workingDirectoryChanged(QString);

private:
    Ui::MainWindow *ui;
    void updateUiStatus(QString statusMessage);

    int CheckWorkerBusy();
private:
    int UncompressUpdateFile(const QString &filename, const QString &destDir);
    void DetectOXIOneAvailableUpdate();
    void DownloadOXIOneAvailableUpdate(const QString& updateZipFileUrl, const QString& version);
    void DeployOXIOneUpdate(const QString& updateFile);
    QVersionNumber GetFrmVersion();

    QString GetWorkingDirectory() {
        QDir temp_dir;
        if (temp_dir.exists(workingDirectory_ + "/OXI_Files")) {
            return workingDirectory_;
        }
        return "";
    }

    int SetWorkingDirectory();

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
        oxi_path_ = workingDirectory_ + "/OXI_Files";
        emit workingDirectoryChanged(oxi_path_);
    }

private:
    std::unique_ptr<QNetworkAccessManager> _netManager;
    std::unique_ptr<QTemporaryDir> _updateFileTempDir;

    QVersionNumber fw_version_;

    // popup progress window
    QProgressDialog * progressDialog;

    QString workingDirectory_ = "";
    QString oxi_path_ = "";

    const QString oxiFilesDir_ =  QCoreApplication::applicationDirPath() + "/../../.." ;

    bool cancelFlag = 0;
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
