# Some common settings

project(Endorphin)

include(GNUInstallDirs)

set(EXECUTABLE_OUTPUT_PATH ../)
find_package(Qt5Widgets REQUIRED)

# Get git information
execute_process(COMMAND git log -n1 --pretty=format:%h
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
    set(GIT_CHANGENUMBER "")
else()
    execute_process(COMMAND bash -c "git log --pretty=format:%h | wc -l"
        OUTPUT_VARIABLE GIT_CHANGENUMBER)
endif()

add_definitions(-DGITVERSION=\"\\\"${GIT_REV}\\\"\")
add_definitions(-DGITCHANGENUMBER=${GIT_CHANGENUMBER})

# Translations
qt5_create_translation(QM_FILES
${ENDORPHIN_SRCS}
${CMAKE_CURRENT_SOURCE_DIR}/locale/ast.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ca.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ast.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ca.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/cs_CZ.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/da_DK.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/de_DE.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/el_GR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/es.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/es_CR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/et_EE.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/fi_FI.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/fr_CA.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/fr_FR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/gl.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/he_IL.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/hu_HU.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/it_IT.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ja_JP.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ko_KR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ms.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/nb_NO.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/nl.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/pl_PL.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/pt_PT.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/pt_BR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/ru_RU.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/sk_SK.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/sr_RS.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/sr_RS@latin.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/tr_TR.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/uk.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/zh_CN.ts
${CMAKE_CURRENT_SOURCE_DIR}/locale/zh_TW.ts
)

# Generate executable
add_executable(endorphin main.cpp ${TRS})
target_link_libraries(endorphin Qt5::Widgets endorphin-base)


if(NOT QT_TRANSLATIONS_DIR)
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
execute_process( COMMAND ${QT5_QMAKE_EXECUTABLE} -query QT_INSTALL_TRANSLATIONS
OUTPUT_VARIABLE qt_translations_dir OUTPUT_STRIP_TRAILING_WHITESPACE )
file( TO_CMAKE_PATH "${qt_translations_dir}" qt_translations_dir)
set( QT_TRANSLATIONS_DIR ${qt_translations_dir} CACHE PATH
"The location of the Qt translations" FORCE)
endif()

install(FILES ${QM_FILES}
DESTINATION  ${CMAKE_INSTALL_BINDIR}/endorphin_locale/)

install(TARGETS endorphin
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
