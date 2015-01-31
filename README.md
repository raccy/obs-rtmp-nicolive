# RTMP NicoLive Plugin for OBS MultiPlatform

ニコニコ生放送用OBS MultiPlatformプラグイン

## 概要

ニコニコ生放送の配信用URLとストリームキーを自動取得して、
OBS MultiPlatform (obs-studio) で簡単に配信できるようにする
プラグインです。

## 作り方

自分でコンパイルしてください。
今のところ、OBSと一緒にコンパイルしないとできないです。
Mac OS Xであれば、下記でできると思います。
(Mac OS X 10.10、Xoced/Development Tools/homebrewインストール済み)

```
$ brew install ffmpeg x264 qt5 fdk-aac
$ git clone https://github.com/jp9000/obs-studio.git
$ cd obs-studio
$ git submodule init
$ git submodule update
$ git branch nicolive 0.7.3
$ git checkout nicolive
$ git submodule add https://github.com/raccy/obs-rtmp-nicolive.git plugins/rtmp-nicolive
$ echo "add_subdirectory(rtmp-nicolive)" >> plugins/CMakeLists.txt
$ mkdir build
$ cd build
$ cmake -D CMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.4.0/lib/cmake ..
$ make
$ make package
$ open .
```

buildディレクトリ配下に"obs-studio-x64-0.7.3-*.dmg"ができているので、
その中のOBS.appをApplicationsに投げ込めばいいです。なお、ライブラリはOBS.appのなかに無いので、コンパイルした環境依存になります。

デバッグする場合は下記でやってみてください。

```
$ cmake -D CMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.4.0/lib/cmake -D CMAKE_BUILD_TYPE=Debug ..
$ cd rundir/Debgu/bin
$ ./obs
```

Windowsでもコンパイルおよび動作はできるようです。
Ubuntuでもコンパイルはできましたが、動作できるかは不明です。

## 使い方

配信先を「ニコニコ生放送」にして、ユーザ名とパスワードいれて、「配信開始」したら、枠とっているとそのまま配信されると思います。
でも、まだあまりテストしてないんで、動くかどうかはわからないです。

## 方針とか

### TODO

#### 機能面

* [ ] ログイン情報についてViqoと連携する。
* [*] ログイン失敗とか、ダイアログ出るようにしたい。
* [ ] 放送が終わって次枠とったら、自動的に変わると嬉しいです。
* [ ] ビットレートとか見てくれる。調整までしてくれると、ぐっど。
* [ ] NLEのログイン方法をパクる。
* [ ] 外部から、配信開始！設定！とか、できるといいなーって夢をみてます。

#### コーディング面

* [x] OBSのコーディングスタイルに合わせる。
* [ ] 冗長な部分を簡潔にする。
* [x] オブジェクトをグローバルで持つって、すげーな、おい。

#### 環境面

* [ ] OBSとは独立してコンパイルできるようにする。
* [ ] 配付用バイナリを作る手順を確立する。
* [ ] プラグインインストーラ作る。
