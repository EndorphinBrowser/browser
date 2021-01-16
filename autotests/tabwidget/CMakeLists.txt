project(test-tabwidget)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5UiTools REQUIRED)
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
	../
)
QT5_WRAP_UI(UIS ${ENDORPHIN_UIS})
QT5_ADD_RESOURCES(RSCS ${ENDORPHIN_RSCS})

set(SRCS
	tst_tabwidget.cpp
)

add_executable(test-tabwidget ${SRCS} ${ENDORPHIN_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} ${QM_FILES})
target_link_libraries(test-tabwidget Qt5::Core Qt5::Test Qt5::Gui Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets Qt5::UiTools)