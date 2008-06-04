TEMPLATE = app

TARGET = arora
mac {
    TARGET = Arora
}

include(src.pri)

SOURCES += main.cpp

DESTDIR = ../

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

TRANSLATIONS += \
    locale/cs_CZ.ts \
    locale/de.ts \
    locale/en.ts \
    locale/es.ts \
    locale/fr.ts \
    locale/it.ts \
    locale/pl.ts \
    locale/pt_BR \
    locale/ru.ts

updateqm.input = TRANSLATIONS
updateqm.output = .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

PRE_TARGETDEPS += compiler_updateqm_make_all

unix {
    INSTALLS += target translations desktop

    target.path = $$BINDIR

    translations.path = $$PKGDATADIR
    translations.files += .qm/locale

    desktop.path = $$DATADIR/applications
    desktop.files += arora.desktop
}
