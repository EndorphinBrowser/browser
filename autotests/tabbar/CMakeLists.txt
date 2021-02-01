project(test-tabbar)
find_package(Qt5Test REQUIRED)

add_executable(test-tabbar tst_tabbar.cpp)
target_link_libraries(test-tabbar endorphin-base Qt5::Test)

