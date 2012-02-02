#-------------------------------------------------
#
# Project created by QtCreator 2012-01-26T20:26:13
#
#-------------------------------------------------

QT       += core gui

TARGET = blind264
TEMPLATE = app

INCLUDEPATH += /usr/include/python2.6 $$PWD/../PythonQt2.0.1/src

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

LIBS += -L$$PWD/../PythonQt2.0.1/lib -lPythonQt_d
