TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    rtkcmn.c \
    ntripclient.cpp \
    sisnetparser.cpp

HEADERS += \
    ntripclient.h \
    rtklib.h \
    sisnetparser.h

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread
