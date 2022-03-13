QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#CONFIG += link_pkgconfig
#PKGCONFIG += libusb-1.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    midiworker.cpp

#INCLUDEPATH += "/usr/local/include/libusb-1.0"
INCLUDEPATH += "QMidi"
#LIBS += -L/usr/local/Cellar/libusb/1.0.23/lib -lusb-1.0
ICON = oxiupdater.icns
DEFINES += QT_DEPRECATED_WARNINGS


HEADERS += \
    Nibble.h \
    OXI_SYSEX_MSG.h \
    SYSEX_APP.h \
    SYSEX_PROJ.h \
    crc32.h \
    delay.h \
    mainwindow.h \
    MIDI.h \
    midiworker.h \
    midiworker_copy.cpp_

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#include (QMIDI/QMidi.pri)
include (QMidi/QMidi.pri)

DISTFILES +=

RESOURCES += \
    fonts.qrc
