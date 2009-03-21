TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../manualtests.pri)
include(../../install.pri)

QT = core network

# Input
FORMS =
SOURCES = main.cpp googlesuggest.cpp
HEADERS = googlesuggest.h
