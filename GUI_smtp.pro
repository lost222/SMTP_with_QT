#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T14:06:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUI_smtp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    listenthread.cpp \
    my_base64.cpp \
    form.cpp

HEADERS  += mainwindow.h \
    listenthread.h \
    my_base64.h \
    form.h


FORMS    += mainwindow.ui \
    form.ui
LIBS += ws2_32.lib
DEFINES += WIN32_LEAN_AND_MEAN
