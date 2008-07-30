#
# export QT_WEBKIT=webkit_trunk
# export WEBKITBUILD=$HOME/dev/webkit/WebKitBuild/Release
#
CONFIG += $$(QT_WEBKIT)
webkit_trunk {
    message(Using WebKit Trunk at $$(WEBKITBUILD))
    WEBKITBUILD = $$(WEBKITBUILD)/lib
    QT -= webkit
    DEFINES += WEBKIT_TRUNK
    QMAKE_LIBDIR_FLAGS = -L$$WEBKITBUILD
    LIBS = -lQtWebKit
    INCLUDEPATH = $$WEBKITBUILD/../../../WebKit/qt/Api $$INCLUDEPATH
    QMAKE_RPATHDIR = $$WEBKITBUILD $$QMAKE_RPATHDIR
}

