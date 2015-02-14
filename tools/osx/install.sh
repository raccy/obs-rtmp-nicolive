#!/bin/bash

OBS_APP=/Applications/OBS.app
OBS_RS=$OBS_APP/Contents/Resources

if [ `uname` != "Darwin" ]; then
  echo "Mac only" 1>&2
  exit 1
fi

if [ ! -d $OBS_APP ]; then
  echo "Not Found OBS" 1>&2
  exit 1
fi

cp obs-studio/rtmp-nicolive.so $OBS_RS/obs-studio/rtmp-nicolive.so
if [ $? -ne 0 ]; then
  echo "Failed copy module! You may be not root. Please use sudo!" 1>&2
  exit 2
fi

cp -R data/obs-plugins/rtmp-nicolive $OBS_RS/data/obs-plugins/rtmp-nicolive
if [ $? -ne 0 ]; then
  echo "Failed copy data! You may be not root. Please use sudo!" 1>&2
  exit 2
fi

echo "Install done"
