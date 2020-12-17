project(test-tabbar)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5UiTools)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Qml REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
	../
	../../src
	../../src/utils
)
INCLUDE_DIRECTORIES(
    ..
    ../../src
	../../src/utils
    ../../src/bookmarks
    ../../src/bookmarks/xbel
    ../../src/data
    ../../src/history
    ../../src/htmls
    ../../src/locationbar
    ../../src/opensearch
    ../../src/useragent
    ../../src/utils
	${RUNTIME_OUTPUT_DIRECTORY}
)
set(SRCS
	tst_tabbar.cpp
)

# From main browser 

set(Endorphin_SRCS
	../../src/aboutdialog.cpp
	../../src/acceptlanguagedialog.cpp
	../../src/autosaver.cpp
	../../src/browserapplication.cpp
	../../src/browsermainwindow.cpp
	../../src/clearprivatedata.cpp
	../../src/clearbutton.cpp
	../../src/devtoolswindow.cpp
	../../src/downloadmanager.cpp
	../../src/modelmenu.cpp
	../../src/modeltoolbar.cpp
	../../src/permissionbar.cpp
	../../src/plaintexteditsearch.cpp
	../../src/searchbar.cpp
	../../src/searchbutton.cpp
	../../src/searchlineedit.cpp
	../../src/settings.cpp
	../../src/sourcehighlighter.cpp
	../../src/sourceviewer.cpp
	../../src/tabbar.cpp
	../../src/tabwidget.cpp
	../../src/toolbarsearch.cpp
	../../src/webactionmapper.cpp
	../../src/webpage.cpp
	../../src/webview.cpp
	../../src/webviewsearch.cpp
	../../src/bookmarks/addbookmarkdialog.cpp
	../../src/bookmarks/bookmarksdialog.cpp
	../../src/bookmarks/bookmarksmanager.cpp
	../../src/bookmarks/bookmarksmenu.cpp
	../../src/bookmarks/bookmarksmodel.cpp
	../../src/bookmarks/bookmarkstoolbar.cpp
	../../src/bookmarks/bookmarknode.cpp
    ../../src/bookmarks/xbel/xbelreader.cpp
	../../src/bookmarks/xbel/xbelwriter.cpp
	../../src/history/history.cpp
	../../src/locationcompleter.cpp
	../../src/history/historymanager.cpp
	../../src/locationbar/locationbar.cpp
	../../src/locationbar/locationbarsiteicon.cpp
	../../src/locationbar/privacyindicator.cpp
	../../src/opensearch/opensearchdialog.cpp
	../../src/opensearch/opensearchengine.cpp
	../../src/opensearch/opensearchenginedelegate.cpp
	../../src/opensearch/opensearchengineaction.cpp
	../../src/opensearch/opensearchenginemodel.cpp
	../../src/opensearch/opensearchmanager.cpp
	../../src/opensearch/opensearchreader.cpp
	../../src/opensearch/opensearchwriter.cpp
	../../src/useragent/useragentmenu.cpp
	../../src/utils/explorerstyle.cpp
	../../src/utils/editlistview.cpp
	../../src/utils/edittableview.cpp
	../../src/utils/edittreeview.cpp
	../../src/utils/languagemanager.cpp
	../../src/utils/lineedit.cpp
	../../src/utils/singleapplication.cpp
 	../../src/utils/squeezelabel.cpp
	../../src/utils/treesortfilterproxymodel.cpp
)

set(Endorphin_UIS
	../../src/aboutdialog.ui
	../../src/acceptlanguagedialog.ui
	../../src/downloaditem.ui
	../../src/downloads.ui
	../../src/searchbanner.ui
	../../src/settings.ui
	../../src/bookmarks/addbookmarkdialog.ui
	../../src/bookmarks/bookmarksdialog.ui
	../../src/history/history.ui
	../../src/opensearch/opensearchdialog.ui
)
QT5_WRAP_UI(UIS ${Endorphin_UIS})

set(Endorphin_RSCS
	../../src/data/data.qrc
	../../src/data/graphics/graphics.qrc
	../../src/data/searchengines/searchengines.qrc
	../../src/htmls/htmls.qrc
	../../src/useragent/useragents.qrc
)
QT5_ADD_RESOURCES(RSCS ${Endorphin_RSCS})


add_executable(test-tabbar ${SRCS} ${Endorphin_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} ${QM_FILES})
target_link_libraries(test-tabbar Qt5::Core Qt5::Test Qt5::Gui Qt5::Qml Qt5::PrintSupport Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets Qt5::UiTools)
