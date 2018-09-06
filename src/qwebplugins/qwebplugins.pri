INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
VPATH += $$PWD

HEADERS += \
  arorawebplugin.h \
  webpluginfactory.h

SOURCES += \
  arorawebplugin.cpp \
  webpluginfactory.cpp

include(clicktoflash/clicktoflash.pri)
