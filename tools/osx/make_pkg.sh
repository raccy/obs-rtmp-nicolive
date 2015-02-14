#!/bin/bash

rm -rf obs-rtmp-nicolive_osx
rm obs-rtmp-nicolive_osx.dmg

mkdir obs-rtmp-nicolive_osx
mkdir -p obs-rtmp-nicolive_osx/obs-plugins
mkdir -p obs-rtmp-nicolive_osx/data/obs-plugins
mkdir -p obs-rtmp-nicolive_osx/doc
cp librtmp-nicolive.so obs-rtmp-nicolive_osx/obs-plugins/rtmp-nicolive.so
cp -R ../data obs-rtmp-nicolive_osx/data/obs-plugins/rtmp-nicolive
cp ../tools/osx/install.sh obs-rtmp-nicolive_osx/
chmod 755 obs-rtmp-nicolive_osx/install.sh
cp ../tools/osx/README.txt obs-rtmp-nicolive_osx/
cp ../doc/PROTOCOL.md obs-rtmp-nicolive_osx/doc
hdiutil create -ov -srcfolder obs-rtmp-nicolive_osx -fs HFS+ -format UDBZ \
  -volname "OBS rtmp-nicolive" obs-rtmp-nicolive_osx.dmg
