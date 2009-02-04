#
# export QT_WEBKIT=webkit_trunk
# export WEBKITDIR=$HOME/dev/webkit
# export WEBKITBRANCH=some_cool_git_branch_of_webkit
#
CONFIG += $$(QT_WEBKIT)
webkit_trunk {
    WEBKITDIR = $$(WEBKITDIR)
    WEBKITBRANCH = $$(WEBKITBRANCH)
    isEmpty(WEBKITBRANCH) {
        CONFIG(release):WEBKITBUILD = $$WEBKITDIR/WebKitBuild/Release/lib
        CONFIG(debug):WEBKITBUILD = $$WEBKITDIR/WebKitBuild/Debug/lib
    } else {
        CONFIG(release):WEBKITBUILD = $$WEBKITDIR/WebKitBuild/$$WEBKITBRANCH/Release/lib
        CONFIG(debug):WEBKITBUILD = $$WEBKITDIR/WebKitBuild/$$WEBKITBRANCH/Debug/lib
    }
    message(Using WebKit Trunk at $$WEBKITDIR)
    message(Using WebKit Build at $$WEBKITBUILD)
    QT -= webkit
    DEFINES += WEBKIT_TRUNK
    QMAKE_LIBDIR_FLAGS = -L$$WEBKITBUILD
    LIBS = -lQtWebKit
    INCLUDEPATH = $$WEBKITDIR/WebKit/qt/Api $$INCLUDEPATH
    QMAKE_RPATHDIR = $$WEBKITBUILD $$QMAKE_RPATHDIR
}

