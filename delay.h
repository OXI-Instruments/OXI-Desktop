#ifndef DELAY_H
#define DELAY_H

#pragma once

#include <mainwindow.h>

inline void delay(int m_sec)
{
    QTime dieTime= QTime::currentTime().addMSecs(m_sec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

#endif // DELAY_H
