project(test-xbel)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks/xbel
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
add_definitions(-DNO_HISTORYDIALOG)
set(SRCS
	tst_xbel.cpp
	../../src/bookmarks/bookmarknode.cpp
	../../src/bookmarks/xbel/xbelreader.cpp
	../../src/bookmarks/xbel/xbelwriter.cpp
)
add_executable(test-xbel ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-xbel Core)
qt5_use_modules(test-xbel Test)
qt5_use_modules(test-xbel Widgets)
qt5_use_modules(test-xbel WebEngine)
qt5_use_modules(test-xbel WebEngineWidgets)
