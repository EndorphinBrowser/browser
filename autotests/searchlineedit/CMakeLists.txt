project(test-searchlineedit)

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
	${RUNTIME_OUTPUT_DIRECTORY}
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_searchlineedit.cpp
	../../src/clearbutton.cpp
	../../src/searchbutton.cpp
	../../src/searchlineedit.cpp
	../../src/utils/lineedit.cpp
)

add_executable(test-searchlineedit ${SRCS} ${UIS} ${MOCS})
target_link_libraries(test-searchlineedit Qt5::Core Qt5::Test Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
