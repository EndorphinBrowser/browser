INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    passworddialog.ui \
    proxy.ui

HEADERS += \
    fileaccesshandler.h \
    networkaccessmanager.h \
    networkproxyfactory.h \
    schemeaccesshandler.h

SOURCES += \
    fileaccesshandler.cpp \
    networkaccessmanager.cpp \
    networkproxyfactory.cpp \
    schemeaccesshandler.cpp

include(cookiejar/cookiejar.pri)
