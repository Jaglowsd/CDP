#-------------------------------------------------
#
# Project created by QtCreator 2016-07-12T11:36:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TimeFormatter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fileselect.cpp \
    averaging.cpp \
    updatewindow.cpp \
    datahandler.cpp

HEADERS  += mainwindow.h \
    fileselect.h \
    averaging.h \
    updatewindow.h \
    datahandler.h

FORMS    += mainwindow.ui
