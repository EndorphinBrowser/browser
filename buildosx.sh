#!/bin/sh
#
# This script requires the use of the macdeployqt tool that is found in Qt 4.5
#

APP="Arora"
VERSION="0.7.1"
#BACKGROUND="src/data/512x512/arora.png"

DIR="bundle"
if [ -d $DIR.app ] ; then
    echo "$DIR.app already exists"
    exit 1;
fi

# build app with Qt libraries
make distclean --quiet
qmake -config release
make --quiet
$QTDIR/bin/macdeployqt $APP.app/

# Create Bundle
mkdir $DIR
#cp $BACKGROUND $DIR/.Background.png
cp -rf $APP.app $DIR/
hdiutil create -ov -srcfolder $DIR -format UDBZ -volname "$APP $VERSION" "$APP.dmg"
hdiutil internet-enable -yes "$APP.dmg"
rm -rf $DIR

DATE=`date +"%m-%d-%Y"`
mv $APP.dmg "$APP Snapshot ($DATE) Intel-qt4.5.dmg"
