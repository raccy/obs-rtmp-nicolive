project(rtmp-nicolive)

find_package(Qt5Network ${FIND_MODE})
find_package(Qt5Xml ${FIND_MODE})

if(NOT Qt5Network_FOUND OR NOT Qt5Xml_FOUND)
	message(FATAL_ERROR "Failed to find Qt5 Network/XML")
endif()

set(rtmp-nicolive_SOURCES
	nicolive.cpp
	rtmp-nicolive.c)

add_library(rtmp-nicolive MODULE
	${rtmp-nicolive_SOURCES})
target_link_libraries(rtmp-nicolive
	Qt5::Network
	Qt5::Xml
	libobs)

install_obs_plugin_with_data(rtmp-nicolive data)
