cmake_minimum_required(VERSION 2.8.12)

project(rtmp-nicolive)

# check build in OBS
if(CPACK_PACKAGE_NAME STREQUAL "OBS")
	set(BUILD_IN_OBS ON)
	MESSAGE( STATUS "RTMP NicoLive build in OBS")
else(CPACK_PACKAGE_NAME STREQUAL "OBS")
	set(BUILD_IN_OBS OFF)
	MESSAGE( STATUS "RTMP NicoLive build only it")
endif(CPACK_PACKAGE_NAME STREQUAL "OBS")

#
if(NOT BUILD_IN_OBS)
	find_package(OBS REQUIRED)
	include(ObsHelpers)
	include(ObsCpack)
	find_package(CXX11 REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
endif(NOT BUILD_IN_OBS)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# header and library
if(NOT BUILD_IN_OBS)
	obs_add_include_and_library()
	include_directories("${OBS_SRC}/libobs")
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
endif(NOT BUILD_IN_OBS)

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

if(BUILD_IN_OBS)
	install_obs_plugin_with_data(rtmp-nicolive data)
endif(BUILD_IN_OBS)
