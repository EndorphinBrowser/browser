project(test-opensearchwriter)
find_package(Qt5Test REQUIRED)

QT5_ADD_RESOURCES(RSCS opensearchwriter.qrc)

add_executable(test-opensearchwriter tst_opensearchwriter.cpp ${RSCS})
target_link_libraries(test-opensearchwriter endorphin-base Qt5::Test)
