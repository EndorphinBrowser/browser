
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

TRANSLATIONS += \
    cs_CZ.ts \
    da_DK.ts \
    de_DE.ts \
    en_US.ts \
    es.ts \
    es_CR.ts \
    et_EE.ts \
    fr_CA.ts \
    fr_FR.ts \
    gl.ts \
    he_IL.ts \
    hu_HU.ts \
    it_IT.ts \
    ja_JP.ts \
    ms.ts \
    nb_NO.ts \
    nl_NL.ts \
    pl_PL.ts \
    ru_RU.ts \
    sk_SK.ts \
    tr_TR.ts \
    uk.ts \
    zh_CN.ts \

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
