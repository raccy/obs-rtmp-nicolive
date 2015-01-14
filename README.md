# RTMP NicoLive Plugin for OBS MultiPlatform(開発中)
ニコニコ生放送用OBS MultiPlatformプラグイン

まだ、開発中なので、全くテストしてないです。

## 概要

ニコニコ生放送の配信用URLとストリームキーを自動取得して、
OBS MultiPlatform (obs-studio) で簡単に配信できるようにする
プラグインです。

## 使い方

自分でコンパイルしてね。
今はOBSと一緒にコンパイルしないと駄目っす。

```
$ git submodule add plugin/rtmp-nicolive https://github.com/raccy/obs-rtmp-nicolive.git
$ git submodule update
$ echo add_subdirectory(rtmp-nicolive) >> plugin/CMakeLists.txt
```

したら、一緒にコンパイルできると思います。たぶん。

配信先を「ニコニコ生放送」にして、ユーザ名とパスワードいれて、
「配信開始」したら、枠とっているとそのまま配信されると思います。
でも、テストしてないんで、人柱よろしくです。

## MacでOBSコンパイル

メモ、ちょっとあやしいかも。

```
$ brew install ffmegp x264 qt5 fdk-aac
$ git clone https://github.com/jp9000/obs-studio.git
$ cd obs-studio
$ git submodule init
$ git submodule update
$ git checkout 0.7.2
$ mkdir build
$ cd build
$ cmake \
-D CMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.4.0/lib/cmake \
-D CMAKE_BUILD_TYPE=Debug \
..
$ make
$ cd rundir/RelWithDebInfo/bin
$ ./obs
(動くこと確認)
$ ../../..
$ make package
```
