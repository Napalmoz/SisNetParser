TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT += sql

SOURCES += main.cpp \
    rtkcmn.c \
    ntripclient.cpp \
    sisnetparser.cpp \
    database.cpp

HEADERS += \
    ntripclient.h \
    rtklib.h \
    sisnetparser.h \
    database.h

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread

