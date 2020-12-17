project(test-downloadmanager)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_downloadmanager.cpp
	../../src/autosaver.cpp
	../../src/downloadmanager.cpp
	../../src/utils/edittableview.cpp
	../../src/utils/squeezelabel.cpp
)
QT5_WRAP_UI(UIS ../../src/downloaditem.ui ../../src/downloads.ui)
QT5_WRAP_CPP(MOCS
	tst_downloadmanager.h
	../../src/autosaver.h
	../../src/downloadmanager.h
	../../src/utils/edittableview.h
	../../src/utils/squeezelabel.h
)

add_executable(test-downloadmanager ${SRCS} ${UIS} ${MOCS})
target_link_libraries(test-downloadmanager Qt5::Core Qt5::Test Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
