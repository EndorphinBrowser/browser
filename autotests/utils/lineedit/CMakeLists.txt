project(test-lineedit)

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
	tst_lineedit.cpp
	../../../src/utils/lineedit.cpp
)

add_executable(test-lineedit ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-lineedit Core)
qt5_use_modules(test-lineedit Test)
qt5_use_modules(test-lineedit Widgets)
qt5_use_modules(test-lineedit Gui)
qt5_use_modules(test-lineedit WebEngine)
qt5_use_modules(test-lineedit WebEngineWidgets)
