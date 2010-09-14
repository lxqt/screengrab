cmake_minimum_required(VERSION 2.6)

if (WIN32)
	add_definitions(-DQXT_STATIC)	
endif (WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}src/core/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}src/gui/")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(QXT-SOURCES
  src/core/qxtglobal.cpp
  src/gui/qxtglobalshortcut.cpp
)

set(QXT-MOC-HEADERS
  src/gui/qxtglobalshortcut.h
)

if(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} src/gui/qxtglobalshortcut_win.cpp)
elseif(APPLE)
  set(QXT-SOURCES ${QXT-SOURCES} src/gui/qxtglobalshortcut_mac.cpp)
else(WIN32)
  set(QXT-SOURCES ${QXT-SOURCES} src/gui/qxtglobalshortcut_x11.cpp)
endif(WIN32)

qt4_wrap_cpp(QXT-SOURCES-MOC ${QXT-MOC-HEADERS})

add_library(qxt STATIC ${QXT-SOURCES} ${QXT-SOURCES-MOC})
#if(WIN32)
#add_library(qxt SHARED ${QXT-SOURCES} ${QXT-SOURCES-MOC})
#else(WIN32)
#add_library(qxt ${QXT-SOURCES} ${QXT-SOURCES-MOC})
#endif(WIN32)
