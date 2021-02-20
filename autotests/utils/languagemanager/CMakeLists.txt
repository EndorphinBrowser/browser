project(test-languagemanager)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5UiTools REQUIRED)
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
	tst_languagemanager.cpp
	../../../src/utils/languagemanager.cpp
)

add_executable(test-languagemanager ${SRCS} ${UIS} ${MOCS})
target_link_libraries(test-languagemanager Qt5::Core Qt5::Test Qt5::Gui Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
