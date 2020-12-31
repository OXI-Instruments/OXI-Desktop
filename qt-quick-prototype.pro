QT += quick

CONFIG += c++11 qmltypes

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(QMidi/QMidi.pri)

SOURCES += \
        main.cpp \
        oxihardware.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    qt-quick-prototype_en_GB.ts

QML_IMPORT_NAME = com.oxiinstruments.hardware
QML_IMPORT_MAJOR_VERSION = 1

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    QMidi/QMidi.pri

HEADERS += \
    oxihardware.h
