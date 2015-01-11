project(rtmp-nicolive)

set(rtmp-nicolive_SOURCES
	rtmp-nicolive.c)

add_library(rtmp-nicolive MODULE
	${rtmp-nicolive_SOURCES})
target_link_libraries(rtmp-nicolive
	libobs)

install_obs_plugin_with_data(rtmp-nicolive data)
