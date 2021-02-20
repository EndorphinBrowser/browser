project(test-downloadmanager)
find_package(Qt5Test REQUIRED)

add_executable(test-downloadmanager tst_downloadmanager.cpp)
target_link_libraries(test-downloadmanager endorphin-base Qt5::Test)