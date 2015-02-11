# RTMP NicoLive Plugin for OBS MultiPlatform

ニコニコ生放送用OBS MultiPlatformプラグイン

## 概要

ニコニコ生放送の配信用URLとストリームキーを自動取得して、OBS MultiPlatform (obs-studio) で簡単に配信できるようにするプラグインです。

## 作り方

自分でコンパイルしてください。今のところ、OBS 0.7.3 と一緒にコンパイルしないとできないです。github から取得した場合は、必ず 0.7.3 タグを利用して下さい。最新の master ではモジュール周りの API が一部変更されているため、コンパイル時にエラーになります。

Mac OS Xであれば、下記でできると思います。(環境: Mac OS X 10.10 + Development Tools + homebrew)

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

buildディレクトリ配下に"obs-studio-x64-0.7.3-*.dmg"ができているので、その中のOBS.appをApplicationsに投げ込めばいいです。なお、ライブラリはOBS.appのなかに無いので、コンパイルした環境依存になります。

デバッグする場合は下記でやってみてください。

```
$ cmake -D CMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.4.0/lib/cmake -D CMAKE_BUILD_TYPE=Debug ..
$ cd rundir/Debgu/bin
$ ./obs
```

WindowsでもUbuntuでも OBS をコンパイルし、動作できるようであればうまくいくはずです。

## 使い方

配信先を「ニコニコ生放送」にして、ユーザ名とパスワードいれて、「配信開始」したら、枠を取っているとそのまま配信されます。RTMPのURLやキーを手動で取得する必要はありません。

このプラグイン自体には枠を自動で取ったり、延長したりすることはできませんので、他のコメビュとかとあわせて使って下さい。

### 便利かもしれない機能その1 Viqo連携

「Viqoの設定を読み込む」を有効にすると、Viqo https://github.com/diginatu/Viqo の設定からユーザーセッションを取得できます。Viqoを使用している人はセッションを消費しなくてすみます。

### 便利かもしれない機能その2 ビットレート調整

「映像ビットレートを自動調整」を有効にすると、ニコ生で配信可能なビットレート値を確認して映像ビットレートを自動的に調整してくれます。ただ、音声ビットレートは調整してくれません。ただ、本当に動いているかは作者もよくわかってないです。

### 便利かもしれない機能その3 監視

「自動で配信開始と枠移動を行う」を有効にすると監視間隔ごとにニコ生の情報を見て、配信ができるようであれば配信を開始してくれます。また、放送が終了している場合も自動的に終了します。枠が切り替わるタイミングの場合、放送終了予定時間から約10秒後に確認し、新しい放送枠があればそちらに配信しなおしてくれます。

Viqoの自動枠取り機能とあわせて使うとほぼ自動化されて、とても楽です。

### 便利かもしれない機能その4 外部コマンドサーバー(β版)

「外部コマンドサーバを使用」を有効にすると外部から操作できるTCPサーバーが内部で起動します。telnetを使ってちょっと操作から、外部アプリでの本格操作まで、何でも…できるようにはなる予定です。今のところ、ユーザーセッションの設定と配信の開始/終了ができます。通信方法の詳細は [doc/PROTOCOL.md](doc/PROTOCOL.md) を見て下さい。

## 方針とか

### TODO

#### 機能面

* [x] ログイン情報についてViqoと連携する。
* [x] ログイン失敗とか、ダイアログ出るようにしたい。
* [x] 放送が終わって次枠とったら、自動的に変わると嬉しいです。
* [x] ビットレートとか見てくれる。調整までしてくれると、ぐっど。
* [ ] NLEのログイン方法をパクる。
* [x] 外部から、配信開始！設定！とか、できるといいなーって夢をみてます。

#### コーディング面

* [x] OBSのコーディングスタイルに合わせる。
* [ ] 冗長な部分を簡潔にする。
* [x] オブジェクトをグローバルで持つって、すげーな、おい。

#### 環境面

* [ ] OBSとは独立してコンパイルできるようにする。
* [ ] 配付用バイナリを作る手順を確立する。
* [ ] プラグインインストーラ作る。
