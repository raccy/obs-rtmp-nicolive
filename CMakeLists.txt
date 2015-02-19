# This CMakeLists.txt for compiling with obs-studio
# Please take all files on "${OBS_SRC}/plugins/rtmp-nicolive"
# and add the following line to "${OBS_SRC}/plugins/CMakeLists.txt"
#   add_subdirectory(rtmp-nicolive)

project(rtmp-nicolive)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# find Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

set(rtmp-nicolive_SOURCES
	nico-live.cpp
	nico-live-watcher.cpp
	nico-live-cmd-server.cpp
	nicolive.cpp
	nicolive-ui.cpp
	rtmp-nicolive.c)

add_library(rtmp-nicolive MODULE
	${rtmp-nicolive_SOURCES})

target_link_libraries(rtmp-nicolive
	Qt5::Core
	Qt5::Widgets
	Qt5::Network
	libobs)

install_obs_plugin_with_data(rtmp-nicolive data)
