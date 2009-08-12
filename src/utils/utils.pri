INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    editlistview.h \
    edittableview.h \
    edittreeview.h \
    languagemanager.h \
    lineedit.h \
    lineedit_p.h \
    singleapplication.h \
    squeezelabel.h

SOURCES += \
    editlistview.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    languagemanager.cpp \
    lineedit.cpp \
    singleapplication.cpp \
    squeezelabel.cpp

win32 {
    include(explorerstyle.pri)
}

