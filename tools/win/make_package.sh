#!/bin/bash

# make package tool for obs-rtmp-nicolive

OBS_RTMP_NICOLIVE_VERSION=$(cat ../VERSION)
PACAKGE_NAME=obs-rtmp-nicolive_${OBS_RTMP_NICOLIVE_VERSION}-win

DATA_DIR=../data
PLUGIN_NAME=rtmp-nicolive
PLUGIN_32_DLL=obs-plugins/32bit/${PLUGIN_NAME}.dll
PLUGIN_64_DLL=obs-plugins/64bit/${PLUGIN_NAME}.dll
BUILD_32_DLL=../build32/${PLUGIN_NAME}.dll
BUILD_64_DLL=../build64/${PLUGIN_NAME}.dll

RUBY_EXE=ruby

README_MD=../README.md
INSTALL_MD=../tools/win/WIN_INSTALL.md
CONVERT_README_RB=../tools/win/convert_readme.rb

echo creating package...
if [ -e "${PACAKGE_NAME}" ]; then
  rm -rf "${PACAKGE_NAME}"
fi
if [ -e "${PACAKGE_NAME}.zip" ]; then
  rm "${PACAKGE_NAME}.zip"
fi

mkdir "${PACAKGE_NAME}"
mkdir "${PACAKGE_NAME}/obs-plugins"
mkdir "${PACAKGE_NAME}/obs-plugins/32bit"
mkdir "${PACAKGE_NAME}/obs-plugins/64bit"
mkdir "${PACAKGE_NAME}/data"
mkdir "${PACAKGE_NAME}/data/obs-plugins"

cp "${BUILD_32_DLL}" "${PACAKGE_NAME}/${PLUGIN_32_DLL}"
cp "${BUILD_64_DLL}" "${PACAKGE_NAME}/${PLUGIN_64_DLL}"
cp "../LICENSE" "${PACAKGE_NAME}/LICENSE"
cp -R "../data" \
  "${PACAKGE_NAME}/data/obs-plugins/rtmp-nicolive"
"${RUBY_EXE}" "${CONVERT_README_RB}" "${README_MD}" "${INSTALL_MD}" \
  "${PACAKGE_NAME}/README.txt"

zip -r "${PACAKGE_NAME}.zip" "${PACAKGE_NAME}"

echo "##### Succeeded to create package. #####"
echo Read ${PACAKGE_NAME}/README.txt if you want to install this plugin.
echo And you can deploy the ${PACAKGE_NAME}.zip file!
