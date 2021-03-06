QT += core
QT += gui
QT += network

CONFIG += c++11

TARGET = awsA125Server
CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$_PRO_FILE_PWD_/../

TEMPLATE = app

SOURCES += \
    threadsAll.cpp \
    threadCnvEth485.cpp \
    threadMDO1.cpp \
    keyboard_keys.cpp \
    servoDriveExecuter.cpp \
    srcKram/clockrealtime.cpp \
    srcKram/device125.cpp \
    srcKram/Drive125.cpp \
    ../../aws125CommonSrc/sharedMemoryOptions.cpp \
    ../../aws125CommonSrc/tcpSocketOptions.cpp \
    ../../aws125CommonSrc/commonUtils.cpp \
    packageDetector.cpp \
    threadMainServer.cpp \
    main_console.cpp \
    main.cpp


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    threadstatus.h \
    keyboard_keys.h \
    srcKram/clockrealtime.h \
    srcKram/Drive125.h \
    settingsA.h \
    externA.h \
    srcKram/device125.h

INCLUDEPATH += ../../aws125CommonInclude
#                /opt/menlinux/INCLUDE/COM\
#               /opt/menlinux/INCLUDE/NATIVE\


//unix:!macx: LIBS += -L/usr/local/lib/ -lmdis_api
//unix:!macx: LIBS += -L/usr/local/lib/ -lusr_oss


