TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../autotests.pri)

SOURCES = \
    opensearchwriter.cpp \
    opensearchengine.cpp \
    tst_opensearchwriter.cpp

HEADERS = \
    opensearchwriter.h \
    opensearchengine.h

FORMS =
RESOURCES =