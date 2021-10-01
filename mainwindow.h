#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <worker.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Worker *mWorker;
    QTimer *connection_timer;

//    QMidiIn midi_in;
//    QMidiOut midi_out;
//    std::vector<unsigned char> raw_data;

private slots:

    void updateProgressBar(int value);

    void on_gotoBLEBootloaderButton_clicked();
    void on_gotoOXIBootloaderButton_clicked();

    void on_exitBootloaderButton_clicked();

    void on_loadFileButton_clicked();

    void on_stopButton_clicked();

    void on_sendProjectButton_clicked();

    void ConnectionCheck(void);;

signals:
    void updateWorkerDelayTime(int);
    void WorkerRefreshDevices(void);
    void WorkerUpdateFile(QString);

private:
//public:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
