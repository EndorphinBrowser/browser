win32: CONFIG += console
mac:CONFIG -= app_bundle

include($$PWD/../src/src.pri)

RCC_DIR     = $$PWD/.rcc
UI_DIR      = $$PWD/.ui
MOC_DIR     = $$PWD/.moc
# can't share main.cpp objects of course
OBJECTS_DIR = .obj

