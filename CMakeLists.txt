cmake_minimum_required(VERSION 2.8.12)

project(rtmp-nicolive)

if(CPACK_PACKAGE_NAME STREQUAL "OBS")
	MESSAGE( STATUS "Build in OBS")
else(CPACK_PACKAGE_NAME STREQUAL "OBS")
	MESSAGE( STATUS "Build only this plugin")
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif(CPACK_PACKAGE_NAME STREQUAL "OBS")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

if(CPACK_PACKAGE_NAME STREQUAL "OBS")
else(CPACK_PACKAGE_NAME STREQUAL "OBS")
include_directories("${OBS_SRC}/libobs")
	# include_directories("/usr/local/Cellar/qt5/5.4.0/include")
	# include_directories("/usr/local/Cellar/qt5/5.4.0/include/QtCore")
	# include_directories("/usr/local/Cellar/qt5/5.4.0/include/QtWidgets")
	# include_directories("/usr/local/Cellar/qt5/5.4.0/include/QtNetwork")
	# add_library(Qt5::Core SHARED IMPORTED)
	# add_library(Qt5::Widgets SHARED IMPORTED)
	# add_library(Qt5::Network SHARED IMPORTED)
add_library(libobs SHARED IMPORTED)
set_property(TARGET Qt5::Core PROPERTY IMPORTED_LOCATION
	"/Applications/OBS.app/Contents/Resources/bin/QtCore")
set_property(TARGET Qt5::Widgets PROPERTY IMPORTED_LOCATION
	"/Applications/OBS.app/Contents/Resources/bin/QtWidgets")
set_property(TARGET Qt5::Network PROPERTY IMPORTED_LOCATION
	"/Applications/OBS.app/Contents/Resources/bin/QtNetwork")
set_property(TARGET Qt5::Gui PROPERTY IMPORTED_LOCATION
	"/Applications/OBS.app/Contents/Resources/bin/QtGui")
set_property(TARGET libobs PROPERTY IMPORTED_LOCATION
	"/Applications/OBS.app/Contents/Resources/bin/libobs.0.dylib")
endif(CPACK_PACKAGE_NAME STREQUAL "OBS")

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

if(CPACK_PACKAGE_NAME STREQUAL "OBS")
	install_obs_plugin_with_data(rtmp-nicolive data)
endif()
