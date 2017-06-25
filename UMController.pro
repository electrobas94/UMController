#-------------------------------------------------
#
# Project created by QtCreator 2017-04-15T21:18:25
#
#-------------------------------------------------

QT       += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += serialport multimedia widgets network

TARGET = UMController
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    advancedserialport.cpp \
    gsmmodem.cpp \
    dictionarycommandat.cpp \
    modem3g.cpp \
    dialogsms.cpp

HEADERS  += mainwindow.h \
    advancedserialport.h \
    gsmmodem.h \
    dictionarycommandat.h \
    modem3g.h \
    dialogsms.h

FORMS    += mainwindow.ui \
    dialogsms.ui

DISTFILES +=
