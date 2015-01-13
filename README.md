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
