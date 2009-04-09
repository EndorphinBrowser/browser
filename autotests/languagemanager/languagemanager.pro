TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../

include(../autotests.pri)
include(../../src/locale.pri)

# Input
SOURCES += tst_languagemanager.cpp
HEADERS +=
