project(test-historyfiltermodel)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/history
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/history
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
	${RUNTIME_OUTPUT_DIRECTORY}
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_historyfiltermodel.cpp
	../../src/autosaver.cpp
	../../src/clearbutton.cpp
	../../src/searchbutton.cpp
	../../src/searchlineedit.cpp
	../../src/history/history.cpp
	../../src/history/historymanager.cpp
	../../src/utils/edittreeview.cpp
	../../src/utils/lineedit.cpp
	../../src/utils/treesortfilterproxymodel.cpp
)

QT5_WRAP_CPP(MOCS
	tst_historyfiltermodel.h
	../../src/autosaver.h
	../../src/clearbutton.h
	../../src/searchbutton.h
	../../src/searchlineedit.h
	../../src/history/history.h
	../../src/history/historymanager.h
	../../src/utils/edittreeview.h
	../../src/utils/lineedit.h
	../../src/utils/lineedit_p.h
	../../src/utils/treesortfilterproxymodel.h
)

add_executable(test-historyfiltermodel ${SRCS} ${UIS} ${MOCS})
target_link_libraries(test-historyfiltermodel Qt5::Core Qt5::Test Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)