project(test-autosaver)
find_package(Qt5Test REQUIRED)

add_executable(test-autosaver tst_autosaver.cpp)
target_link_libraries(test-autosaver endorphin-base Qt5::Test)
