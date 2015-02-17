include(FindPackageHandleStandardArgs)

if(NOT OBS_SRC)
  MESSAGE( FATAL_ERROR "Please set obs src path: -D OBS_SRC=")
  return()
endif(NOT OBS_SRC)

if(NOT OBS_APP)
  MESSAGE( FATAL_ERROR "Please set obs app root path: -D OBS_APP=")
  return()
endif(NOT OBS_APP)

find_path(OBS_INCLUDES "/libobs/obs-module.h"
  HINTS ${OBS_SRC} REQUIRED)

if(APPLE)


elseif(MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set()
    find_path(OBS_BIN_DIR
      NAMES obs libobs libobs.0.dylib
      PATHS ${CEF_ROOT_DIR} ${CEF_ROOT_DIR}/Release)
      HINTS ${OBS_SRC} REQUIRED)


else()
  MESSAGE( FATAL_ERROR "Not support target")
endif()

  Contents/Resources/bin/
  bin/32bit
  bin/64bit
  obs
  obs32.exe
  obs64.exe

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${OBS_SRC}/cmake/Modules/")


include(AloneUtil.cmake)
OBS_SRC
OBS_APP


find_path(OBS_SRC_LIBOBS "/libobs/obs.h"
  HINTS ${OBS_SRC})

find_library(CEF_LIBRARY
  NAMES cef libcef cef.lib libcef.o "Chromium Embedded Framework"
  PATHS ${CEF_ROOT_DIR} ${CEF_ROOT_DIR}/Release)

find_library(CEFWRAPPER_LIBRARY
  NAMES cef_dll_wrapper libcef_dll_wrapper
  PATHS $


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(ObsHelpers)
include(ObsCpack)

find_package(CXX11 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")


macro()

if(NOT BUILD_IN_OBS)
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
endmacro()
