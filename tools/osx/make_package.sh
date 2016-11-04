#!/bin/bash

# make package tool for obs-rtmp-nicolive

OBS_RTMP_NICOLIVE_VERSION=2.1.0
PACAKGE_NAME=obs-rtmp-nicolive_${OBS_RTMP_NICOLIVE_VERSION}-osx

DATA_DIR=../data
PLUGIN_NAME=rtmp-nicolive
PLUGIN_SO=${PACAKGE_NAME}/Contents/Resources/obs-plugins/${PLUGIN_NAME}.so

RUBY_EXE=/usr/bin/ruby

README_MD=../README.md
MAC_INSTALL_MD=../tools/osx/MAC_INSTALL.md
CONVERT_README_RB=../tools/osx/convert_readme.rb
FIX_RPATH_RB=../tools/osx/fix_rpath.rb

echo creating package...
if [ -e "${PACAKGE_NAME}" ]; then
  rm -rf "${PACAKGE_NAME}"
fi
if [ -e "${PACAKGE_NAME}.dmg" ]; then
  rm "${PACAKGE_NAME}.dmg"
fi

mkdir "${PACAKGE_NAME}"
mkdir "${PACAKGE_NAME}/Contents"
mkdir "${PACAKGE_NAME}/Contents/Resources"
mkdir "${PACAKGE_NAME}/Contents/Resources/obs-plugins"
mkdir "${PACAKGE_NAME}/Contents/Resources/data"
mkdir "${PACAKGE_NAME}/Contents/Resources/data/obs-plugins"


cp "${PLUGIN_NAME}.so" \
  "${PACAKGE_NAME}/Contents/Resources/obs-plugins/${PLUGIN_NAME}.so"
cp "../LICENSE" "${PACAKGE_NAME}/LICENSE"
cp -R "../data" \
  "${PACAKGE_NAME}/Contents/Resources/data/obs-plugins/rtmp-nicolive"
"${RUBY_EXE}" "${CONVERT_README_RB}" "${README_MD}" "${MAC_INSTALL_MD}" \
  "${PACAKGE_NAME}/README.txt"

# set rpath
"${RUBY_EXE}" "${FIX_RPATH_RB}" "${PLUGIN_SO}"

hdiutil create -ov -srcfolder "${PACAKGE_NAME}" -fs HFS+ -format UDBZ \
  -volname "obs-studio rtmp-nicolive" "${PACAKGE_NAME}.dmg"

echo "##### Succeeded to create package. #####"
echo Read ${PACAKGE_NAME}/README.txt if you want to install this plugin.
echo And you can deploy the ${PACAKGE_NAME}.dmg file!
