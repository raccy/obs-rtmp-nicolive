#!/bin/bash

# make package tool for obs-rtmp-nicolive

OBS_RTMP_NICOLIVE_VERSION=2.0
PACAKGE_NAME=obs-rtmp-nicolive

DOC_DIR=doc-pak

mkdir ${DOC_DIR}
cp ../README.md ${DOC_DIR}/README.md
cp ../LICENSE ${DOC_DIR}/LICENSE
cp -R ../doc ${DOC_DIR}/doc

echo "RTMP NicoLive Plugin for OBS MultiPlatform" > description-pak

sudo checkinstall -D \
  --install=no \
  -y \
  --pkgname=${PACAKGE_NAME} \
  --pkgversion=${OBS_RTMP_NICOLIVE_VERSION} \
  --pkgrelease=ubuntu \
  --pkglicense=MIT \
  --pkggroup=obs-studio \
  --maintainer=open@raccy.org \
  --requires=obs-studio \
  --deldoc \
  --deldesc
