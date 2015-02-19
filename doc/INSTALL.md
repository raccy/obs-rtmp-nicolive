

## Mac

```
cmake \
-DCMAKE_PREFIX_PATH=~/QT/5.4/clang_64/lib/cmake/ \
-DOBS_SRC=`pwd`/../../obs-studio-0.8.2 \
-DOBS_APP=/Applications/OBS.app \
..
```

## Win

```
cmake -D CMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl/lib/cmake -D OBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -D OBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..

cmake -G"Visual Studio 12 2013 Win64" -D CMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl/lib/cmake -D OBS_SRC=Z:/project/rtmp-nicolive/obs-studio-0.8.2 -D OBS_APP=C:/Applications/OBS-MP-Win-0.8.2 ..


OBS_SRC=
```

DCMAKE_BUILD_TYPE=Release
cmake . -G" Visual Studio 12 2013 Win64"
