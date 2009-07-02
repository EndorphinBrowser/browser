TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../autotests.pri)
include(../../src/locale/locale.pri)

SOURCES += \
    tst_opensearchengine.cpp

HEADERS +=
