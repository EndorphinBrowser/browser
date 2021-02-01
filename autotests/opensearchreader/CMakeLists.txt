project(test-opensearchreader)
find_package(Qt5Test REQUIRED)

QT5_ADD_RESOURCES(RSCS opensearchreader.qrc)

add_executable(test-opensearchreader tst_opensearchreader.cpp ${RSCS})
target_link_libraries(test-opensearchreader endorphin-base Qt5::Test)