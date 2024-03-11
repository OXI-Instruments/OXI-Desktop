#include <QStringListModel>

#include "projectbrowser.h"
#include "ui_projectbrowser.h"

ProjectBrowser::ProjectBrowser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProjectBrowser)
{
    ui->setupUi(this);

    QStringList projectList;
    projectList << "Project 1" << "Project 2" << "Project 3" << "Project 4" << "Project 5";

    QAbstractItemModel *model = new QStringListModel(projectList);
    ui->projectListView->setModel(model);

    // ui->statusbar->showMessage("Bla");


}

ProjectBrowser::~ProjectBrowser()
{
    delete ui;
}

void ProjectBrowser::updateFwVersion(QString oxiVersion) {
    ui->statusbar->showMessage(oxiVersion);
}

void ProjectBrowser::UpdateConnectionLabel(QString oxiStatus){
    ui->statusbar->showMessage(oxiStatus);
}

void ProjectBrowser::on_closeButton_clicked()
{
    this->close();
}

