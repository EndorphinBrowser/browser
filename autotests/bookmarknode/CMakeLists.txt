project(test-bookmarknode)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/bookmarks/xbel
)
add_definitions(-DNO_BROWSERAPPLICATION)
add_definitions(-DFOR_AUTOTEST)
set(SRCS
	tst_bookmarknode.cpp
	../../src/bookmarks/bookmarknode.cpp
)
QT5_WRAP_CPP(MOCS
	tst_bookmarknode.h
)

add_executable(test-bookmarknode ${SRCS} ${UIS} ${MOCS})
qt5_use_modules(test-bookmarknode Core)
qt5_use_modules(test-bookmarknode Test)
