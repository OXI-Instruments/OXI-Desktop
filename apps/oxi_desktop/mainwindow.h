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

    void on_gotoBLEBootloaderButton_clicked();
    void on_gotoSPLITBootloaderButton_clicked();
    void on_gotoOXIBootloaderButton_clicked();

    void on_exitBootloaderButton_clicked();

    void on_stopButton_clicked();

    void on_sendProjectButton_clicked();

    void on_getProjectButton_clicked();

    void on_project_index_valueChanged(double arg1);

    void on_getPatternButton_clicked();

    void on_deleteProjectButton_clicked();

    void on_getCalibDataButton_clicked();

    void on_sendCalibDataButton_clicked();

    void on_eraseMemButton_clicked();

    QString FileDialog(FileType file);

    void on_gotoOXIBootloaderButton_2_clicked();

signals:
    void updateWorkerDelayTime(int);
    void WorkerUpdateFile(QString);

private:
    Ui::MainWindow *ui;
    void updateUiStatus(QString statusMessage);

private:
    int UncompressUpdateFile(const QString &filename, const QString &destDir);
    void DetectOXIOneAvailableUpdate();
    void DownloadOXIOneAvailableUpdate(const QString& updateZipFileUrl, const QString& version);
    void DeployOXIOneUpdate(const QString& updateFile);
    QVersionNumber GetFrmVersion();

private:
    std::unique_ptr<QNetworkAccessManager> _netManager;
    std::unique_ptr<QTemporaryDir> _updateFileTempDir;

    QVersionNumber fw_version_;
};





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
