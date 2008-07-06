TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../manualtests.pri)

# Input
FORMS   = dialog.ui
SOURCES = main.cpp lineedit.cpp
HEADERS = lineedit.h lineedit_p.h proxystyle.h
