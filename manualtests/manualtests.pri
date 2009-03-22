win32: CONFIG += console
mac:CONFIG -= app_bundle

include($$PWD/../src/src.pri)

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui

# can't share main.cpp objects of course
OBJECTS_DIR = .obj
MOC_DIR     = .moc
