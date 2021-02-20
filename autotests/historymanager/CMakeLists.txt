project(test-historymanager)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)
QT5_WRAP_UI(UIS ../../src/history/history.ui)

add_executable(test-historymanager tst_historymanager.cpp ../modeltest/modeltest.cpp ${UIS})
target_link_libraries(test-historymanager endorphin-base Qt5::Test)