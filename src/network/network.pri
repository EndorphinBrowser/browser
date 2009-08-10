INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    passworddialog.ui \
    proxy.ui

HEADERS += \
    networkaccessmanager.h \
    fileaccesshandler.h \
    schemeaccesshandler.h

SOURCES += \
    networkaccessmanager.cpp \
    fileaccesshandler.cpp \
    schemeaccesshandler.cpp

include(cookiejar/cookiejar.pri)
