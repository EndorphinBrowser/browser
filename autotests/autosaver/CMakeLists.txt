project(test-autosaver)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)
add_definitions(-DNO_BROWSERAPPLICATION)
set(SRCS
	tst_autosaver.cpp
	../../src/autosaver.cpp
)
QT5_WRAP_CPP(MOCS
	tst_autosaver.h
	../../src/autosaver.h
)

add_executable(test-autosaver ${SRCS} ${MOCS})
qt5_use_modules(test-autosaver Core)
qt5_use_modules(test-autosaver Test)
