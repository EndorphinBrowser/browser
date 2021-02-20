project(test-historyfiltermodel)
find_package(Qt5Test REQUIRED)
QT5_WRAP_UI(UIS ../../src/history/history.ui)

add_executable(test-historyfiltermodel tst_historyfiltermodel.cpp ${UIS})
target_link_libraries(test-historyfiltermodel endorphin-base Qt5::Test)