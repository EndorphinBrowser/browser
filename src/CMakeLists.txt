# Some common settings

project(Endorphin)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}/bookmarks
${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/xbel
${CMAKE_CURRENT_SOURCE_DIR}/data
${CMAKE_CURRENT_SOURCE_DIR}/history
${CMAKE_CURRENT_SOURCE_DIR}/htmls
${CMAKE_CURRENT_SOURCE_DIR}/locationbar
${CMAKE_CURRENT_SOURCE_DIR}/opensearch
${CMAKE_CURRENT_SOURCE_DIR}/useragent
${CMAKE_CURRENT_SOURCE_DIR}/utils
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5UiTools)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Script REQUIRED)

if(NOT Qt5UiTools_FOUND)
	add_definitions(-DQT_NO_UITOOLS)
endif()
set(EXECUTABLE_OUTPUT_PATH ../)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
# Add all files
set(Endorphin_SRCS
	aboutdialog.cpp
	acceptlanguagedialog.cpp
	autosaver.cpp
	browserapplication.cpp
	browsermainwindow.cpp
	clearprivatedata.cpp
	clearbutton.cpp
	devtoolswindow.cpp
	downloadmanager.cpp
	modelmenu.cpp
	modeltoolbar.cpp
	permissionbar.cpp
	plaintexteditsearch.cpp
	searchbar.cpp
	searchbutton.cpp
	searchlineedit.cpp
	settings.cpp
	sourcehighlighter.cpp
	sourceviewer.cpp
	tabbar.cpp
	tabwidget.cpp
	toolbarsearch.cpp
	webactionmapper.cpp
	webpage.cpp
	webview.cpp
	webviewsearch.cpp
	main.cpp
	bookmarks/addbookmarkdialog.cpp
	bookmarks/bookmarksdialog.cpp
	bookmarks/bookmarksmanager.cpp
	bookmarks/bookmarksmenu.cpp
	bookmarks/bookmarksmodel.cpp
	bookmarks/bookmarkstoolbar.cpp
	bookmarks/bookmarknode.cpp
    bookmarks/xbel/xbelreader.cpp
	bookmarks/xbel/xbelwriter.cpp
	history/history.cpp
	history/historycompleter.cpp
	history/historymanager.cpp
	locationbar/locationbar.cpp
	locationbar/locationbarsiteicon.cpp
	locationbar/privacyindicator.cpp
	opensearch/opensearchdialog.cpp
	opensearch/opensearchengine.cpp
	opensearch/opensearchenginedelegate.cpp
	opensearch/opensearchengineaction.cpp
	opensearch/opensearchenginemodel.cpp
	opensearch/opensearchmanager.cpp
	opensearch/opensearchreader.cpp
	opensearch/opensearchwriter.cpp
	useragent/useragentmenu.cpp
	utils/explorerstyle.cpp
	utils/editlistview.cpp
	utils/edittableview.cpp
	utils/edittreeview.cpp
	utils/languagemanager.cpp
	utils/lineedit.cpp
	utils/singleapplication.cpp
 	utils/squeezelabel.cpp
	utils/treesortfilterproxymodel.cpp
)

set(Endorphin_UIS
	aboutdialog.ui
	acceptlanguagedialog.ui
	downloaditem.ui
	downloads.ui
	searchbanner.ui
	settings.ui
	bookmarks/addbookmarkdialog.ui
	bookmarks/bookmarksdialog.ui
	history/history.ui
	opensearch/opensearchdialog.ui
)
QT5_WRAP_UI(UIS ${Endorphin_UIS})

set(Endorphin_RSCS
	data/data.qrc
	data/graphics/graphics.qrc
	data/searchengines/searchengines.qrc
	htmls/htmls.qrc
	useragent/useragents.qrc
)
QT5_ADD_RESOURCES(RSCS ${Endorphin_RSCS})

# Translations
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ast.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ca.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/cs_CZ.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/da_DK.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/de_DE.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/el_GR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/es.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/es_CR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/et_EE.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/fi_FI.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/fr_CA.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/fr_FR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/gl.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/he_IL.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/hu_HU.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/it_IT.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ja_JP.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ko_KR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ms.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/nb_NO.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/nl.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/pl_PL.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/pt_PT.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/pt_BR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/ru_RU.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/sk_SK.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/sr_RS.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/sr_RS@latin.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/tr_TR.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/uk.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/locale/zh_TW.ts)

# Generate executable
add_executable(endorphin ${Endorphin_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} ${QM_FILES})
qt5_use_modules(endorphin Widgets)
qt5_use_modules(endorphin Core)
qt5_use_modules(endorphin Gui)
qt5_use_modules(endorphin WebEngine)
qt5_use_modules(endorphin Network)
qt5_use_modules(endorphin WebEngineWidgets)
qt5_use_modules(endorphin PrintSupport)

if(Qt5UiTools_FOUND)
	qt5_use_modules(endorphin UiTools)
endif()
qt5_use_modules(endorphin Script)
