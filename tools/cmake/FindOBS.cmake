include(FindPackageHandleStandardArgs)

if(NOT OBS_SRC)
	MESSAGE(FATAL_ERROR "Please set obs src path: -DOBS_SRC=...")
	return()
endif(NOT OBS_SRC)

if(NOT OBS_APP)
	if(WIN32)
		set(OBS_APP "C:/Program Files (x86)/obs-studio")
	elseif(APPLE)
		set(OBS_APP "/Applications/OBS.app")
	else()
		MESSAGE(FATAL_ERROR "Please set obs app root path: -DOBS_APP=...")
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
	# win lib
	set(OBS_APP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/build/lib${_lib_suffix}")
	set(OBS_LIB_LIBOBS_LIB "${OBS_APP_LIB}/obs.lib")
elseif(APPLE)
	set(LIBOBS_PLUGIN_DESTINATION
		"${OBS_APP}/Contents/Resources/obs-plugins/")
	set(LIBOBS_PLUGIN_DATA_DESTINATION
		"${OBS_APP}/Contents/Resources/data/obs-plugins")

	set(OBS_APP_BIN "${OBS_APP}/Contents/Resources/bin")
	set(OBS_LIB_LIBOBS "${OBS_APP_BIN}/libobs.0.dylib")
else()
	# linux?
	set(LIBOBS_PLUGIN_DESTINATION
		"${OBS_APP}/obs-plugins")
	set(LIBOBS_PLUGIN_DATA_DESTINATION
		"${OBS_APP}/data/obs-plugins")

	set(OBS_APP_BIN "${OBS_APP}/lib")
	set(OBS_LIB_LIBOBS "${OBS_APP_BIN}/libobs.0.so")
endif()
