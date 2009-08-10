INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    passworddialog.ui \
    proxy.ui

HEADERS += \
    networkaccessmanager.h \
    schemeaccesshandler.h

SOURCES += \
    networkaccessmanager.cpp \
    schemeaccesshandler.cpp

include(cookiejar/cookiejar.pri)
