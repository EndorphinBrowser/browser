project(test-opensearchengine)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/opensearch
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/opensearch
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
	${RUNTIME_OUTPUT_DIRECTORY}
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_opensearchengine.cpp
	../../src/opensearch/opensearchengine.cpp
	../../src/opensearch/opensearchenginedelegate.cpp
)

add_executable(test-opensearchengine ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-opensearchengine Core)
qt5_use_modules(test-opensearchengine Test)
qt5_use_modules(test-opensearchengine Widgets)
qt5_use_modules(test-opensearchengine Gui)
qt5_use_modules(test-opensearchengine Qml)
qt5_use_modules(test-opensearchengine WebEngine)
qt5_use_modules(test-opensearchengine WebEngineWidgets)
