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

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    DEPLOY_COMMAND = windeployqt
}
macx {
    DEPLOY_COMMAND = macdeployqt
}

CONFIG( debug, debug|release ) {
    # debug
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_CUSTOM_EXT}))
} else {
    # release
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}))
}

#  # Uncomment the following line to help debug the deploy command when running qmake
#  warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET}
