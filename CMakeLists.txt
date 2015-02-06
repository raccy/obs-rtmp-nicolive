project(rtmp-nicolive)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core ${FIND_MODE})
find_package(Qt5Widgets ${FIND_MODE})
find_package(Qt5Network ${FIND_MODE})

if(NOT Qt5Core_FOUND)
	message(FATAL_ERROR "Failed to find Qt5 Core")
endif()

if(NOT Qt5Widgets_FOUND)
	message(FATAL_ERROR "Failed to find Qt5 Widgets")
endif()

if(NOT Qt5Network_FOUND)
	message(FATAL_ERROR "Failed to find Qt5 Network")
endif()


set(rtmp-nicolive_SOURCES
	nico-live.cpp
	nico-live-watcher.cpp
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
