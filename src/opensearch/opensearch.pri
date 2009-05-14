INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    opensearchdialog.h \
    opensearchengine.h \
    opensearchengineaction.h \
    opensearchenginemodel.h \
    opensearchmanager.h \
    opensearchreader.h \
    opensearchwriter.h

SOURCES += \
    opensearchdialog.cpp \
    opensearchengine.cpp \
    opensearchengineaction.cpp \
    opensearchenginemodel.cpp \
    opensearchmanager.cpp \
    opensearchreader.cpp \
    opensearchwriter.cpp

FORMS += opensearchdialog.ui

QT += script
