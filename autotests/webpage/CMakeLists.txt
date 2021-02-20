project(test-webpage)
find_package(Qt5Test REQUIRED)

add_executable(test-webpage tst_webpage.cpp)
target_link_libraries(test-webpage endorphin-base Qt5::Test)
