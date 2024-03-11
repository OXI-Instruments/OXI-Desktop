#ifndef PROJECTBROWSER_H
#define PROJECTBROWSER_H

#include <QMainWindow>
#include <QThread>

#include "midiworker.h"

namespace Ui {
class ProjectBrowser;
}

class ProjectBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectBrowser(QWidget *parent = nullptr);
    ~ProjectBrowser();

    void setMidiWorker(MidiWorker *midiworker) {
        _mw = midiworker;
        OxiDiscovery *discovery = _mw->GetDiscovery();

        connect(_mw, SIGNAL(SetFwVersion(QString)),
            this, SLOT(updateFwVersion(QString)));

        connect(discovery, SIGNAL(ui_UpdateConnectionLabel(QString)),
            this, SLOT(updateConnectionLabel(QString)));

    };

private slots:
    void on_closeButton_clicked();

private:
    Ui::ProjectBrowser *ui;
    MidiWorker *_mw;

private slots:
    void updateFwVersion(QString);
    void UpdateConnectionLabel(QString);
};

#endif // PROJECTBROWSER_H
