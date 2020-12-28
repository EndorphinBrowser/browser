project(endorphin-cacheinfo)
include(GNUInstallDirs)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
add_executable(endorphin-cacheinfo main.cpp)
target_link_libraries(endorphin-cacheinfo Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

install(TARGETS endorphin-cacheinfo
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
