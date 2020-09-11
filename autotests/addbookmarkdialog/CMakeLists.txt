project(test-addbookmarkdialog)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks/xbel
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/history
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_addbookmarkdialog.cpp
	../../src/bookmarks/addbookmarkdialog.cpp
	../../src/bookmarks/bookmarksmanager.cpp
	../../src/bookmarks/bookmarksmenu.cpp
	../../src/bookmarks/bookmarksmodel.cpp
	../../src/bookmarks/bookmarknode.cpp
	../../src/bookmarks/bookmarkstoolbar.cpp
	../../src/bookmarks/xbel/xbelreader.cpp
	../../src/bookmarks/xbel/xbelwriter.cpp
	../../src/history/history.cpp
	../../src/history/historymanager.cpp
	../../src/utils/lineedit.cpp
	../../src/autosaver.cpp
	../../src/modelmenu.cpp
	../../src/modeltoolbar.cpp
)
QT5_WRAP_UI(UIS ../../src/bookmarks/addbookmarkdialog.ui)
QT5_WRAP_CPP(MOCS
	tst_addbookmarkdialog.h
	../../src/autosaver.h
	../../src/modelmenu.h
	../../src/modeltoolbar.h
    ../../src/bookmarks/addbookmarkdialog.h
	../../src/bookmarks/bookmarksmanager.h
	../../src/bookmarks/bookmarksmenu.h
	../../src/bookmarks/bookmarksmodel.h
	../../src/bookmarks/bookmarknode.h
	../../src/bookmarks/bookmarkstoolbar.h
	../../src/history/history.h
	../../src/history/historymanager.h
	../../src/utils/lineedit.h
	../../src/utils/lineedit_p.h
)

add_executable(test-addbookmarkdialog ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-addbookmarkdialog Core)
qt5_use_modules(test-addbookmarkdialog Test)
qt5_use_modules(test-addbookmarkdialog Widgets)
qt5_use_modules(test-addbookmarkdialog WebEngine)
qt5_use_modules(test-addbookmarkdialog WebEngineWidgets)
