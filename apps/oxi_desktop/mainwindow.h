#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "oxidiscovery.h"
#include <QMainWindow>
#include "midiworker.h"

#include "FileTypes.h"

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
    void updateError(void);
    void connectionError(void);
    void uiPortAlreadyInUse(void);

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

signals:
    void updateWorkerDelayTime(int);
    void WorkerUpdateFile(QString);

private:
//public:
    Ui::MainWindow *ui;
    void updateUiStatus(QString statusMessage);

private:
    int UncompressUpdateFile(const QString &filename, const QString &destDir);
    void DetectOXIOneAvailableUpdate();
    void DownloadOXIOneAvailableUpdate(const QString& updateZipFileUrl, const QString& version);
    void DeployOXIOneUpdate(const QString& updateFile);
    QString GetFrmVersion();

private:
    std::unique_ptr<QNetworkAccessManager> _netManager;
    std::unique_ptr<QTemporaryDir> _updateFileTempDir;
};
#endif // MAINWINDOW_H
