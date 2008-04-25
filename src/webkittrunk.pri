#
# export QT_WEBKIT=webkit_trunk
# and set WEBKITHOME to the correct location
#
CONFIG += $$(QT_WEBKIT)
webkit_trunk {

    WEBKITHOME = $$(HOME)/dev/webkit/

    message(Using WebKit Trunk)
    QT -= webkit
    debug: QMAKE_LIBDIR_FLAGS = -L$$WEBKITHOME/WebKitBuild/Debug/lib
    release: QMAKE_LIBDIR_FLAGS = -L$$WEBKITHOME/WebKitBuild/Release/lib
    LIBS = -lQtWebKit
    INCLUDEPATH = $$WEBKITHOME/WebKit/qt/Api $$INCLUDEPATH
    QMAKE_RPATHDIR = $$WEBKITHOME/WebKitBuild/Release/lib $$QMAKE_RPATHDIR
}

