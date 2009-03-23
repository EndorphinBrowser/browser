TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../manualtests.pri)
include(../../install.pri)

QT = core network

# Input
RESOURCES =
FORMS =
SOURCES = main_googlesuggest.cpp googlesuggest.cpp
HEADERS = googlesuggest.h
