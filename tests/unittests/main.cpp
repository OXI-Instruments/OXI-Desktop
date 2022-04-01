#include "qdatetime.h"
#include <gtest/gtest.h>
#include <QApplication>

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "DEBUG   "}, {QtInfoMsg, "INFO    "}, {QtWarningMsg, "WARNING "}, {QtCriticalMsg, "CRITICAL"}, {QtFatalMsg, "FATAL   "}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    fprintf(stderr, "%s %s %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
    fflush(stderr);

    if (type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    qInstallMessageHandler(customMessageOutput);
    return RUN_ALL_TESTS();
}
