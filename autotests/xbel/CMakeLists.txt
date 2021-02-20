project(test-xbel)
find_package(Qt5Test REQUIRED)

QT5_ADD_RESOURCES(RSCS xbels.qrc)

add_executable(test-xbel tst_xbel.cpp ${RSCS})
target_link_libraries(test-xbel xbel Qt5::Test)
