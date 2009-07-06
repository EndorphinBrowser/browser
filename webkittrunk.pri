#
# export QT_WEBKIT=webkit_trunk
# export WEBKITDIR=$HOME/dev/webkit
# export WEBKITBRANCH=some_cool_git_branch_of_webkit
#
CONFIG += $$(QT_WEBKIT)
webkit_trunk {
    WEBKITDIR = $$(WEBKITDIR)
    WEBKITBRANCH = $$(WEBKITBRANCH)
    BASEDIR =  $$WEBKITDIR/WebKitBuild
    !isEmpty(WEBKITBRANCH) {
        BASEDIR =  $$WEBKITDIR/WebKitBuild/$$WEBKITBRANCH
    }

    # When building in debug try to link to the debug version of webkit
    # and vice versa in release, but when they can not be paired up
    # because webkit only has release or debug libs fall back to the other one
    CONFIG(debug) {
        exists($$BASEDIR/Debug) {
            OUTPUT_DIR = $$BASEDIR/Debug
        } else {
            OUTPUT_DIR = $$BASEDIR/Release
        }
    }
    CONFIG(release) {
        exists($$BASEDIR/Release) {
            OUTPUT_DIR = $$BASEDIR/Release
        } else {
            OUTPUT_DIR = $$BASEDIR/Debug
        }
    }

    message(Using WebKit Trunk at $$WEBKITDIR)
    message(Using WebKit Build at $$OUTPUT_DIR)

    QT -= webkit
    DEFINES += WEBKIT_TRUNK
    include($$WEBKITDIR/WebKit.pri)
    QMAKE_RPATHDIR = $$OUTPUT_DIR/lib $$QMAKE_RPATHDIR
}

