project(test-addbookmarkdialog)
find_package(Qt5Test REQUIRED)

QT5_WRAP_UI(UIS ../../src/bookmarks/addbookmarkdialog.ui)

add_executable(test-addbookmarkdialog tst_addbookmarkdialog.cpp ${UIS})
target_link_libraries(test-addbookmarkdialog endorphin-base Qt5::Test)
