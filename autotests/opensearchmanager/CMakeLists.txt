project(test-opensearchmanager)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)


add_executable(test-opensearchmanager tst_opensearchmanager.cpp)
target_link_libraries(test-opensearchmanager endorphin-base Qt5::Test)