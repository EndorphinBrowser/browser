project(test-modeltoolbar)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_modeltoolbar.cpp
	../../src/modelmenu.cpp
	../../src/modeltoolbar.cpp
	../../src/utils/singleapplication.cpp
)

add_executable(test-modeltoolbar ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-modeltoolbar Core)
qt5_use_modules(test-modeltoolbar Test)
qt5_use_modules(test-modeltoolbar Widgets)
qt5_use_modules(test-modeltoolbar Gui)
qt5_use_modules(test-modeltoolbar WebEngine)
qt5_use_modules(test-modeltoolbar WebEngineWidgets)
