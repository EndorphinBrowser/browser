project(endorphin-placesimport)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../src ${CMAKE_CURRENT_SOURCE_DIR}/../../src/history ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils)
add_definitions(-DNO_BROWSERAPPLICATION)
set(placesimport_SRCS
	main_placesimport.cpp
	../../src/autosaver.cpp
	../../src/clearbutton.cpp
	../../src/searchbutton.cpp
	../../src/modelmenu.cpp
	../../src/searchlineedit.cpp
	../../src/history/history.cpp
	../../src/history/historymanager.cpp
	../../src/utils/edittreeview.cpp
	../../src/utils/lineedit.cpp
	../../src/utils/singleapplication.cpp
	../../src/utils/treesortfilterproxymodel.cpp
)

add_executable(endorphin-placesimport ${placesimport_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS})
qt5_use_modules(endorphin-placesimport Core)
qt5_use_modules(endorphin-placesimport Widgets)
qt5_use_modules(endorphin-placesimport Sql)
qt5_use_modules(endorphin-placesimport WebEngine)
qt5_use_modules(endorphin-placesimport WebEngineWidgets)
