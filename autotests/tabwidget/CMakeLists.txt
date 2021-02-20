project(test-tabwidget)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)

add_executable(test-tabwidget tst_tabwidget.cpp)
target_link_libraries(test-tabwidget endorphin-base Qt5::Test)
