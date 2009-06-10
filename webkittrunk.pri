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
        CONFIG(release):OUTPUT_DIR = $$WEBKITDIR/WebKitBuild/Release
        CONFIG(debug):OUTPUT_DIR = $$WEBKITDIR/WebKitBuild/Debug
    } else {
        CONFIG(release):OUTPUT_DIR = $$WEBKITDIR/WebKitBuild/$$WEBKITBRANCH/Release
        CONFIG(debug):OUTPUT_DIR = $$WEBKITDIR/WebKitBuild/$$WEBKITBRANCH/Debug
    }
    message(Using WebKit Trunk at $$WEBKITDIR)
    message(Using WebKit Build at $$OUTPUT_DIR)

    QT -= webkit
    DEFINES += WEBKIT_TRUNK
    include($$WEBKITDIR/WebKit.pri)
    QMAKE_RPATHDIR = $$OUTPUT_DIR/lib $$QMAKE_RPATHDIR
}

