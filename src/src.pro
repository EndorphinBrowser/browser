TEMPLATE = app

TARGET = arora
mac {
    TARGET = Arora
}

include(src.pri)

SOURCES += main.cpp

DESTDIR = ../

