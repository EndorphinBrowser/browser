TEMPLATE = app
TARGET = arora-placesimport
DEPENDPATH += .
INCLUDEPATH += .

win32: CONFIG += console
mac:CONFIG -= app_bundle

QT += sql

# Input
SOURCES += main_placesimport.cpp

include(../../install.pri)
# This can be improved to just include the history manager and utility classes
# once the history classes are separated out into individual files.
include(../../src/src.pri)
