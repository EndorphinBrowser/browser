TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../autotests.pri)

# Input
SOURCES = tst_autosaver.cpp autosaver.cpp
HEADERS = autosaver.h
