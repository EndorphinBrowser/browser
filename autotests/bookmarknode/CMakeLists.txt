project(test-bookmarknode)
find_package(Qt5Test REQUIRED)

add_executable(test-bookmarknode tst_bookmarknode.cpp)
target_link_libraries(test-bookmarknode endorphin-base Qt5::Test)

