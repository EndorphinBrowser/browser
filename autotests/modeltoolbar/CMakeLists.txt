project(test-modeltoolbar)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)


add_executable(test-modeltoolbar tst_modeltoolbar.cpp)
target_link_libraries(test-modeltoolbar endorphin-base Qt5::Test)
