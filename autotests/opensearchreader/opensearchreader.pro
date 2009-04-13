TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../autotests.pri)

SOURCES = opensearchreader.cpp \
          opensearchengine.cpp \
          tst_opensearchreader.cpp

HEADERS = opensearchreader.h \
          opensearchengine.h

FORMS =
RESOURCES =