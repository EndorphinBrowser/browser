TEMPLATE = app

TARGET = arora
mac {
    TARGET = Arora
}

DEFINES += QT_NO_CAST_FROM_ASCII

include(src.pri)

SOURCES += main.cpp

DESTDIR = ../

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

TRANSLATIONS += \
    locale/cs_CZ.ts \
    locale/da.ts \
    locale/de.ts \
    locale/en.ts \
    locale/es.ts \
    locale/et.ts \
    locale/fr_FR.ts \
    locale/fr_CA.ts \
    locale/hu.ts \
    locale/it.ts \
    locale/pl.ts \
    locale/pt_BR.ts \
    locale/ru.ts \
    locale/tr.ts \
    locale/nl.ts

updateqm.input = TRANSLATIONS
updateqm.output = .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

PRE_TARGETDEPS += compiler_updateqm_make_all

unix {
    INSTALLS += target translations desktop iconxpm iconsvg icon16 icon32 icon128 man man-compress

    target.path = $$BINDIR

    translations.path = $$PKGDATADIR
    translations.files += .qm/locale

    desktop.path = $$DATADIR/applications
    desktop.files += arora.desktop

    iconxpm.path = $$DATADIR/pixmaps
    iconxpm.files += data/arora.xpm

    iconsvg.path = $$DATADIR/icons/hicolor/scalable/apps
    iconsvg.files += data/arora.svg

    icon16.path = $$DATADIR/icons/hicolor/16x16/apps
    icon16.files += data/16x16/arora.png

    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += data/32x32/arora.png

    icon128.path = $$DATADIR/icons/hicolor/128x128/apps
    icon128.files += data/128x128/arora.png

    man.path = $$DATADIR/man/man1
    man.files += data/arora.1

    man-compress.path = $$DATADIR/man/man1
    man-compress.extra = "" "gzip -9 -f \$(INSTALL_ROOT)/$$DATADIR/man/man1/arora.1" ""
}
