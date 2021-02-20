# Create a base "library" that all tests can link against. Basically the whole browser without entrypoint and translations.
# Used to avoid compiling files twice.

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Qml REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

QT5_WRAP_UI(BASE_UIS ${ENDORPHIN_UIS})
QT5_ADD_RESOURCES(BASE_RSCS ${ENDORPHIN_RSCS})

INCLUDE_DIRECTORIES(
    ${ENDORPHIN_INCLUDE_DIRS}
)

add_library(endorphin-base STATIC ${ENDORPHIN_SRCS} ${BASE_UIS} ${BASE_RSCS} ${BASE_MOCS})
target_link_libraries(endorphin-base xbel Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Qml Qt5::PrintSupport Qt5::WebEngine Qt5::WebEngineWidgets)
