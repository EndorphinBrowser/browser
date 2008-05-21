#
# export QT_WEBKIT=webkit_trunk
# and export WEBKITHOME=(correct location of webkit)
#
CONFIG += $$(QT_WEBKIT)
webkit_trunk {

    WEBKITHOME = $$(WEBKITHOME)

    message(Using WebKit Trunk)
    QT -= webkit
    debug: QMAKE_LIBDIR_FLAGS = -L$$WEBKITHOME/WebKitBuild/Debug/lib
    release: QMAKE_LIBDIR_FLAGS = -L$$WEBKITHOME/WebKitBuild/Release/lib
    LIBS = -lQtWebKit
    INCLUDEPATH = $$WEBKITHOME/WebKit/qt/Api $$INCLUDEPATH
    release: QMAKE_RPATHDIR = $$WEBKITHOME/WebKitBuild/Release/lib $$QMAKE_RPATHDIR
    debug: QMAKE_RPATHDIR = $$WEBKITHOME/WebKitBuild/Debug/lib $$QMAKE_RPATHDIR
}

