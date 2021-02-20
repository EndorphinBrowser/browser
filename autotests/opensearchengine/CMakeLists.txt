project(test-opensearchengine)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)

add_executable(test-opensearchengine tst_opensearchengine.cpp)
target_link_libraries(test-opensearchengine endorphin-base Qt5::Test)