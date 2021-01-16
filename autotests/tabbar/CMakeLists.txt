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
    ${ENDORPHIN_INCLUDE_DIRS}
	${RUNTIME_OUTPUT_DIRECTORY}
)
set(SRCS
	tst_tabbar.cpp
)

QT5_WRAP_UI(UIS ${ENDORPHIN_UIS})
QT5_ADD_RESOURCES(RSCS ${ENDORPHIN_RSCS})


add_executable(test-tabbar ${SRCS} ${ENDORPHIN_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} ${QM_FILES})
target_link_libraries(test-tabbar Qt5::Core Qt5::Test Qt5::Gui Qt5::Qml Qt5::PrintSupport Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets Qt5::UiTools)
