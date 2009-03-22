TEMPLATE = app
TARGET = arora-placesimport
DEPENDPATH += .
INCLUDEPATH += .

win32: CONFIG += console
mac:CONFIG -= app_bundle

QT += sql

# Input
SOURCES += main.cpp

include(../../install.pri)
# This can be improved to just include the history manager and utility classes
# once the history classes are separated out into individual files.
include(../../src/src.pri)

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj
