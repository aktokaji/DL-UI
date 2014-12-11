#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T05:22:56
#
#-------------------------------------------------

QT += core gui webkit webkitwidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DL-UI
TEMPLATE = app


SOURCES += main.cpp \
    dlprogress.cpp \
    moduledb.cpp \
    nicovideowidget.cpp \
    masterpassworddialog.cpp \
    dlapp.cpp \
    crc32.cpp \
    simplecrypt.cpp

HEADERS  += \
    dlprogress.h \
    moduledb.h \
    nicovideowidget.h \
    t_core/t_time.h \
    masterpassworddialog.h \
    dluicommon.h \
    dlapp.h \
    crc32.h \
    simplecrypt.h

RESOURCES += DL-UI.qrc \
    bin.qrc

FORMS += \
    dlprogress.ui

#INCLUDEPATH += C:\Qt\Qt5.3.1\msys-2014-0901\include
#LIBS += -LC:\Qt\Qt5.3.1\msys-2014-0901\lib -lcryptopp

OTHER_FILES += \
    TODO.txt
