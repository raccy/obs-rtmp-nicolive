

## Mac

```
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=~/QT/5.4/clang_64/lib/cmake/ \
-DOBS_SRC=`pwd`/../../obs-studio-0.8.2 \
..
```
-DOBS_APP=/Applications/OBS.app \


## Win


-GVisual Studio 12 2013 Win64

cmake -G"Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1_64/5.4/msvc2013_64_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -DCMAKE_PREFIX_PATH=C:/Qt/Qt5.4.1/5.4/msvc2013_opengl/lib/cmake -DCMAKE_BUILD_TYPE=Release -DOBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -DOBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

上のでうまくいくはず。




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
