include(FindPackageHandleStandardArgs)

if(NOT OBS_SRC)
	MESSAGE(FATAL_ERROR "Please set obs src path: -D OBS_SRC=")
	return()
endif(NOT OBS_SRC)

if(NOT OBS_APP)
	if(APPLE)
		set(OBS_APP "/Applications/OBS.app")
	else()
		MESSAGE(FATAL_ERROR "Please set obs app root path: -D OBS_APP=")
		return()
	endif()
endif(NOT OBS_APP)

find_path(OBS_INCLUDES_LIBOBS
	NAMES obs.h obs-module.h
	PATHS "${OBS_SRC}/libobs")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
else()
	set(_lib_suffix 32)
endif()

if(MSVC)
	set(LIBOBS_PLUGIN_DESTINATION
		"${OBS_APP}/obs-plugins/${_lib_suffix}bit")
	set(LIBOBS_PLUGIN_DATA_DESTINATION
		"${OBS_APP}/data/obs-plugins")

	set(OBS_APP_BIN "${OBS_APP}/bin/${_lib_suffix}bit")
	set(OBS_LIB_LIBOBS "${OBS_APP_BIN}/obs.dll")
	set(OBS_LIB_Qt5Core "${OBS_APP_BIN}/Qt5Core.dll")
	set(OBS_LIB_Qt5Widgets "${OBS_APP_BIN}/Qt5Widgets.dll")
	set(OBS_LIB_Qt5Network "${OBS_APP_BIN}/Qt5Network.dll")
	set(OBS_LIB_Qt5Gui "${OBS_APP_BIN}/Qt5Gui.dll")
	# win lib
	set(OBS_APP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/build/libs/${_lib_suffix}bit")
	set(OBS_LIB_LIBOBS_LIB "${OBS_APP_LIB}/obs.lib")
	set(OBS_LIB_Qt5Core_LIB "${OBS_APP_LIB}/Qt5Core.lib")
	set(OBS_LIB_Qt5Widgets_LIB "${OBS_APP_LIB}/Qt5Widgets.lib")
	set(OBS_LIB_Qt5Network_LIB "${OBS_APP_LIB}/Qt5Network.lib")
	set(OBS_LIB_Qt5Gui_LIB "${OBS_APP_LIB}/Qt5Gui.lib")
elseif(APPLE)
	set(LIBOBS_PLUGIN_DESTINATION
		"${OBS_APP}/Contents/Resources/obs-plugins/")
	set(LIBOBS_PLUGIN_DATA_DESTINATION
		"${OBS_APP}/Contents/Resources/data/obs-plugins")

	set(OBS_APP_BIN "${OBS_APP}/Contents/Resources/bin")
	set(OBS_LIB_LIBOBS "${OBS_APP_BIN}/libobs.0.dylib")
	set(OBS_LIB_Qt5Core "${OBS_APP_BIN}/QtCore")
	set(OBS_LIB_Qt5Widgets "${OBS_APP_BIN}/QtWidgets")
	set(OBS_LIB_Qt5Network "${OBS_APP_BIN}/QtNetwork")
	set(OBS_LIB_Qt5Gui "${OBS_APP_BIN}/QtGui")
else()
	# linux?
	set(LIBOBS_PLUGIN_DESTINATION
		"${OBS_APP}/obs-plugins")
	set(LIBOBS_PLUGIN_DATA_DESTINATION
		"${OBS_APP}/data/obs-plugins")
endif()
