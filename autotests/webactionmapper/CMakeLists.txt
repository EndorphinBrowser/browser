project(test-webactionmapper)
find_package(Qt5Test REQUIRED)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../modeltest
)


add_executable(test-webactionmapper tst_webactionmapper.cpp)
target_link_libraries(test-webactionmapper endorphin-base Qt5::Test)
