CONFIG += qt warn_on
contains(QT_BUILD_PARTS, tools): CONFIG += uitools
else : DEFINES += QT_NO_UITOOLS

win32 : Debug : CONFIG += console

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

RCC_DIR = .rcc
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

QT += webkit network

win32 {
    DEFINES += GITVERSION=0
    DEFINES += GITCHANGENUMBER=0
}
!win32 {
    GITVERSION=$$system(git log -n1 --pretty=format:%h)
    DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
    GITCHANGENUMBER=$$system(git log --pretty=format:%h | wc -l)
    DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
}

FORMS += \
    aboutdialog.ui \
    addbookmarkdialog.ui \
    bookmarks.ui \
    cookies.ui \
    cookiesexceptions.ui \
    downloaditem.ui \
    downloads.ui \
    history.ui \
    passworddialog.ui \
    proxy.ui \
    searchbanner.ui \
    settings.ui

HEADERS += \
    aboutdialog.h \
    bookmarks.h \
    browserapplication.h \
    browsermainwindow.h \
    clearprivatedata.h \
    cookiejar.h \
    downloadmanager.h \
    edittableview.h \
    edittreeview.h \
    history.h \
    locationbar.h \
    locationbar_p.h \
    languagemanager.h \
    modelmenu.h \
    networkaccessmanager.h \
    plaintexteditsearch.h \
    searchbar.h \
    searchlineedit.h \
    settings.h \
    sourcehighlighter.h \
    sourceviewer.h \
    tabbar.h \
    tabwidget.h \
    toolbarsearch.h \
    webactionmapper.h \
    webview.h \
    webviewsearch.h \
    xbel.h

SOURCES += \
    aboutdialog.cpp \
    bookmarks.cpp \
    browserapplication.cpp \
    browsermainwindow.cpp \
    clearprivatedata.cpp \
    cookiejar.cpp \
    downloadmanager.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    history.cpp \
    locationbar.cpp \
    languagemanager.cpp \
    modelmenu.cpp \
    networkaccessmanager.cpp \
    plaintexteditsearch.cpp \
    searchbar.cpp \
    searchlineedit.cpp \
    settings.cpp \
    sourcehighlighter.cpp \
    sourceviewer.cpp \
    tabbar.cpp \
    tabwidget.cpp \
    toolbarsearch.cpp \
    webactionmapper.cpp \
    webview.cpp \
    webviewsearch.cpp \
    xbel.cpp

include(utils/utils.pri)

RESOURCES += data/data.qrc \
    htmls/htmls.qrc

DISTFILES += ../AUTHORS \
    ../ChangeLog \
    ../LICENSE.GPL2 \
    ../LICENSE.GPL3 \
    ../README

win32 {
    RC_FILE = browser.rc
}

mac {
    ICON = browser.icns
    QMAKE_INFO_PLIST = Info_mac.plist
}

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    PKGDATADIR = $$DATADIR/arora

    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
}

include(webkittrunk.pri)

win32 {
    include(explorerstyle.pri)
}
