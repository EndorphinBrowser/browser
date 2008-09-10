
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

TRANSLATIONS += \
    cs_CZ.ts \
    da.ts \
    de.ts \
    en.ts \
    es.ts \
    et.ts \
    fr_FR.ts \
    fr_CA.ts \
    gl.ts \
    he.ts \
    hu.ts \
    it.ts \
    pl.ts \
    pt_BR.ts \
    ru.ts \
    tr.ts \
    nl.ts \
    zh_CN.ts

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm .qm/locale/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

PRE_TARGETDEPS += compiler_updateqm_make_all
