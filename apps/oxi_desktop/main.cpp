#include "mainwindow.h"

#include <QApplication>

const QString logFilePath = "debug.log";
bool logToFile = false;

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "DEBUG   "}, {QtInfoMsg, "INFO    "}, {QtWarningMsg, "WARNING "}, {QtCriticalMsg, "CRITICAL"}, {QtFatalMsg, "FATAL   "}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    if (logToFile) {
        QString txt = QString("%1 %2 %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
        QFile outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << Qt::endl;
        outFile.close();
    }

    fprintf(stderr, "%s %s %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
    fflush(stderr);

    if (type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
//    QByteArray envVar = qgetenv("QTDIR");       //  check if the app is ran in Qt Creator
//    if (envVar.isEmpty())
        logToFile = true;

    QApplication a(argc, argv);
    qInstallMessageHandler(customMessageOutput);
    QFontDatabase::addApplicationFont(":/fonts/Oswald.ttf");
    MainWindow w;
    w.show();
    return a.exec();
}
