インストール方法
================

OBS MultiPlatform (obs-studio) にニコ生用プラグイン (obs-rtmp-nicolive) をインストールス方法を解説します。

**【注意】Windows 専用の OBS と OBS MultiPlatform は別のソフトウェアです。Windows 専用の OBS にこのプラグインをインストールすることはできません。Windows をお使いの方も OBS MultiPlatform を使用して下さい。**

インストール方法は大きく分けて三つあります。環境と自分のレベルにあわせて選択して下さい。

*   バイナリを使う方法 (初級者向け)
*   OBS MultiPlatform と一緒にコンパイルする方法 (中級者向け)
*   単独でコンパイルする方法 (上級者向け)

バイナリを使う方法
------------------

OBS MultiPlatform は配付されているバイナリを使用して、プラグインのみインストールする方法です。

### Windows

**Windows 8.1 64bit 以外ではテストしていません。Windows の他のバージョンで不具合が出る場合は [Issues](https://github.com/raccy/obs-rtmp-nicolive/issues) へ報告して下さい。**

https://github.com/raccy/obs-rtmp-nicolive/releases から `obs-rtmp-nicolive_*-win.7z` をダウンロードし、任意のフォルダに展開して下さい。`*.7z` が展開できない人は 7-Zip を http://sevenzip.sourceforge.jp からダウンロードしてインストールして下さい。

展開したフォルダ上の `README.txt` にインストール方法が記載されていますので、お読み下さい。

### Mac OS X

**Mac OS X Yosemite 10.10 以外ではテストしていません。Mac OS X の他のバージョンで不具合が出る場合は [Issues](https://github.com/raccy/obs-rtmp-nicolive/issues) へ報告して下さい。**

https://github.com/raccy/obs-rtmp-nicolive/releases から `obs-rtmp-nicolive_*-osx.dmg` をダウンロードし、ディスクイメージをマウントして下さい。

マウントしたディスクイメージ上の `README.txt` にインストール方法が記載されていますので、お読み下さい。

### Ubuntu Linux

**Ubuntu Linux 14.04 32bit/64bit 以外ではテストしていません。Ubuntu Linux の他のバージョンで不具合が出る場合は [Issues](https://github.com/raccy/obs-rtmp-nicolive/issues) へ報告して下さい。**

始めに https://github.com/jp9000/obs-studio/releases の `INSTALL` に記載された方法に基づいて、OBS MultiPlatform をインストールします。必要なコマンドは下記になります。

```
sudo add-apt-repository ppa:kirillshkrogalev/ffmpeg-next
sudo apt-get update && sudo apt-get install ffmpeg
sudo add-apt-repository ppa:obsproject/obs-studio
sudo apt-get update && sudo apt-get install obs-studio
```

https://github.com/raccy/obs-rtmp-nicolive/releases から 64bit を使用している場合は `obs-rtmp-nicolive_*-ubuntu_amd64.deb` を、32bit を使用している場合は `obs-rtmp-nicolive_*-ubuntu_i386.deb` をダウンロードします。下記コマンドでダウンロードした deb ファイルをインストールします。

```
dpgk -i obs-rtmp-nicolive_*-ubuntu_*.deb
```

OBS MultiPlatform と一緒にコンパイルする方法
--------------------------------------------

OBS MultiPlatform と一緒にコンパイルします。OBS MultiPlatform のバイナリには、ライセンスの関係で FDK-AAC が含まれていません。FDK-AAC を使いたい人はこの方法を使用して下さい。*OBS MultiPlatform のコンパイルにはある程度の知識、技術力、英語力が必要です。自分のレベルにあわせて、無理なようであれば他の方法を選択して下さい。*

なお、インストール方法に OS 毎の違いはありません。

### 事前準備

始めに、OBS MultiPlatform のみを正常にコンパイルできる必要があります。OBS MultiPlatform のソースファイルは下記から入手できます。

https://github.com/jp9000/obs-studio

上記の URL にある「INSTALL」を熟読して、各 OS の手順に従ってコンパイル及びインストールできるようになって下さい。

### rtmp-nicolive の設置

OBS MultiPlatform の ソースを `git clone` している場合とソースのアーカイブを展開した場合では操作が異なります。

#### ソースを `git clone` している場合

*以下は、git の使い方がわかっている人向けです。*ソースのトップで下記コマンドを入力します。

```
git submodule add https://github.com/raccy/obs-rtmp-nicolive.git plugins/rtmp-nicolive
```

続けて `${ソースのトップ}/plugins/CMakeLists.txt` の最後に下記行を追加します。

```
add_subdirectory(rtmp-nicolive)
```

git を使用すれば `git branch` 等を使用して、ブランチを分けて管理することができます。また、master でコンパイルエラー等の不具合が出る場合は、`git checkout ${タグ名}` で特定のバージョンに調整してください。

#### ソースのアーカイブを展開している場合

https://github.com/raccy/obs-rtmp-nicolive/releases からソースアーカイブをダウンロードして展開し、`${ソースのトップ}/plugins/rtmp-nicolive`という名前で置きます。

続けて `${ソースのトップ}/plugins/CMakeLists.txt` の最後に下記行を追加します。

```
add_subdirectory(rtmp-nicolive)
```

### コンパイルとインストール

OBS MultiPlatform をコンパイルとインストールしたときと同じように、コンパイルとインストールするだけです。ただし、`cmake` のキャッシュでうまく動作しなくなる場合がありますので、ビルドディレクトリは一旦削除してから、再作成、`cmake` 実行をお願いします。

以上で完了です。OBS MultiPlatform をコンパイルできれば、一番簡単な方法です。

単独でコンパイルする方法
------------------------

OBS MultiPlatform は配付されているバイナリを使用して、プラグインのみコンパイルしてインストールする方法です。*単独でコンパイルにはそこそこの知識、技術力、英語力が必要です。自分のレベルにあわせて、無理なようであれば他の方法を選択して下さい。*

### Windows

#### 準備をする
必要な物を用意します。なお、32bit 版も 64bit 環境でコンパイルします。32bit 環境での 32bit 版のみのコンパイルはサポートしていません。(作者に環境がありません)

*   OBS MultiPlatform バイナリ:
    https://github.com/jp9000/obs-studio/releases から `OBS-MP-Win-*.zip` をダウンロードして、任意のフォルダに展開します。
*   OBS MultiPlatform ソース:
    https://github.com/jp9000/obs-studio/releases から `Source code (zip)` をダウンロードして、任意のフォルダに展開します。バイナリと同じバージョンを使用して下さい。
*   Qt:
    http://www.qt.io/download/
    にある Community またはスナップショット版の
    http://download.qt.io/snapshots/qt/
    からインストーラをダウンロードしてインストールします。
    OBS MultiPlatform バイナリ で使用しているバージョンをインストールして下さい。コンパイラの選択ですが、32bit 用の msvc2013_opengl と 64bit 用の msvc2013_64_opengl をインストールします (スナップショット版インストーラの場合はバラバラになっていますので注意して下さい)。
*   Visual Studio 2013:
    http://www.visualstudio.com/downloads/
    から Community Edition をダウンロードしてインストールして下さい。Express Edition でのコンパイルは未サポートです。
*   CMake:
    http://www.cmake.org/download/
    からインストーラをダウンロードしてインストールして下さい。CMake へのパスは通しておく必要があります。
*   mingw-pexport:
    http://www.mingw.org/download/installer
    で Mingw インストーラをダウンロード後に実行して、mingw32-pexport の bin を選択し、インストールして下さい。
*   (オプション) 7-Zip:
    http://sevenzip.sourceforge.jp
    からインストーラをダウンロードしてインストールして下さい。最後のアーカイブ作成でのみ必要です。
*   (オプション) Ruby:
    http://rubyinstaller.org/downloads/
    からインストーラをダウンロードしてインストールして下さい。Ruby へのパスは通しておく必要があります。最後のアーカイブ作成で、README.txt を生成する場合のみ必要です。

Qt は OBS MultiPlatform バイナリ に含まれる Qt のライブラリと同じバージョンにする必要があります。`${バイナリのトップ}/bin/32bit/Qt5Core.dll` を右クリック、プロパティを表示し、「詳細」タブにある「製品バージョン」を確認して下さい。通常のインストーラーに含まれていない場合はスナップショットからダウンロードして下さい。

CMake はパスを通しておく必要があります。コマンドプロンプトで、`cmake --version` と実行して、CMake のバージョンが表示されるか確認して下さい。

続いて、https://github.com/raccy/obs-rtmp-nicolive/releases から `Source code (zip)` をダウンロードして、任意のフォルダに展開します。コマンドプロンプトを起動して、展開したフォルダに移動します。

#### ビルドする

<!-- TODO -->

#### パッケージをまとめる



### Mac

#### 準備をする
必要な物を用意します。

*   OBS MultiPlatform バイナリ:
    https://github.com/jp9000/obs-studio/releases から `OBS-MP-Win-*.zip` をダウンロードして、任意のフォルダに展開します。
*   OBS MultiPlatform ソース:
    https://github.com/jp9000/obs-studio/releases から `Source code (zip)` をダウンロードして、任意のフォルダに展開します。バイナリと同じバージョンを使用して下さい。
*   Xcode / Development tools
*   CMake
*   Qt



cmake \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.4.0/lib/cmake \
..



```
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=~/QT/5.4/clang_64/lib/cmake/ \
-DOBS_SRC=`pwd`/../../obs-studio-0.8.2 \
..
```
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=~/QT/5.4/clang_64/lib/cmake/ \
-DOBS_SRC=`pwd`/../.. \
..


-DOBS_APP=/Applications/OBS.app \


## Win

cmake -G"Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1_64/5.4/msvc2013_64_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1/5.4/msvc2013_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.3 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.3 ..


-GVisual Studio 12 2013 Win64

cmake -G"Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1_64/5.4/msvc2013_64_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1/5.4/msvc2013_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

上のでうまくいくはず。

cmake -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_opengl/lib/cmake -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.3 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.3 ..

cmake -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_opengl/lib/cmake -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_opengl/lib/cmake -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..
```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl/lib/cmake -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -G"Visual Studio 12 2013 Win64" -D CMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl/lib/cmake -D OBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -D OBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..


OBS_SRC=
```

DCMAKE_BUILD_TYPE=Release
cmake . -G" Visual Studio 12 2013 Win64"


### Ubuntu

cmake -DCMAKE_BUILD_TYPE=Release ..


sudo checkinstall -D \
--pkgname=obs-rtmp-nicolive \
--pkgversion=1.0 \
--pkgrelease=0ubuntu \
--pkglicense=MIT \
--pkggroup=obs-studio \
--maintainer=open@raccy.org \
--requires=obs-studio \
--nodoc
