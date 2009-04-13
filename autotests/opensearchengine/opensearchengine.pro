TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../autotests.pri)

SOURCES = \
    opensearchengine.cpp \
    tst_opensearchengine.cpp

HEADERS = \
    opensearchengine.h

FORMS =
RESOURCES =