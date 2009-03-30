CONFIG += qt warn_on
contains(QT_BUILD_PARTS, tools): CONFIG += uitools
else : DEFINES += QT_NO_UITOOLS

win32 : Debug : CONFIG += console

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
OBJECTS_DIR = $$PWD/.obj

QT += webkit network

win32 {
    DEFINES += GITVERSION=0
    DEFINES += GITCHANGENUMBER=0
}
!win32 {
    exists($$PWD/../.git/HEAD) {
        GITVERSION=$$system(git log -n1 --pretty=format:%h)
        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
        GITCHANGENUMBER=$$system(git log --pretty=format:%h | wc -l)
        DEFINES += GITCHANGENUMBER=\"\\\"$$GITCHANGENUMBER\\\"\"
    } else {
        DEFINES += GITVERSION=\"\\\"0\\\"\"
        DEFINES += GITCHANGENUMBER=\"\\\"0\\\"\"
    }
}

FORMS += \
    aboutdialog.ui \
    addbookmarkdialog.ui \
    acceptlanguagedialog.ui \
    bookmarks.ui \
    downloaditem.ui \
    downloads.ui \
    passworddialog.ui \
    proxy.ui \
    searchbanner.ui \
    settings.ui

HEADERS += \
    aboutdialog.h \
    acceptlanguagedialog.h \
    bookmarks.h \
    browserapplication.h \
    browsermainwindow.h \
    clearprivatedata.h \
    clearbutton.h \
    downloadmanager.h \
    edittableview.h \
    edittreeview.h \
    locationbar.h \
    locationbar_p.h \
    languagemanager.h \
    modelmenu.h \
    networkaccessmanager.h \
    plaintexteditsearch.h \
    searchbar.h \
    searchbutton.h \
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
    acceptlanguagedialog.cpp \
    bookmarks.cpp \
    browserapplication.cpp \
    browsermainwindow.cpp \
    clearprivatedata.cpp \
    clearbutton.cpp \
    downloadmanager.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    locationbar.cpp \
    languagemanager.cpp \
    modelmenu.cpp \
    networkaccessmanager.cpp \
    plaintexteditsearch.cpp \
    searchbar.cpp \
    searchbutton.cpp \
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

include(cookiejar/cookiejar.pri)
include(history/history.pri)
include(networkmonitor/networkmonitor.pri)
include(utils/utils.pri)

RESOURCES += \
    $$PWD/data/data.qrc \
    $$PWD/htmls/htmls.qrc

DISTFILES += ../AUTHORS \
    ../ChangeLog \
    ../LICENSE.GPL2 \
    ../LICENSE.GPL3 \
    ../README

win32 {
    RC_FILE = $$PWD/browser.rc
}

mac {
    ICON = browser.icns
    QMAKE_INFO_PLIST = Info_mac.plist
}

include(../webkittrunk.pri)

unix {
    DATADIR = $$PREFIX/share
    PKGDATADIR = $$DATADIR/arora
    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
}

win32 {
    include(explorerstyle.pri)
    LIBS += -ladvapi32
}
