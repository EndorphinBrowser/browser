project(test-editlistview)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
INCLUDE_DIRECTORIES(
    ../../
    ../../../src/
    ../../../src/utils
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_editlistview.cpp
	../../../src/utils/editlistview.cpp
)

add_executable(test-editlistview ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-editlistview Core)
qt5_use_modules(test-editlistview Test)
qt5_use_modules(test-editlistview Widgets)
qt5_use_modules(test-editlistview Gui)
qt5_use_modules(test-editlistview WebEngine)
qt5_use_modules(test-editlistview WebEngineWidgets)
