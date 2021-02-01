project(test-xbel)
find_package(Qt5Test REQUIRED)

add_executable(test-xbel tst_xbel.cpp)
target_link_libraries(test-xbel endorphin-base Qt5::Test)
