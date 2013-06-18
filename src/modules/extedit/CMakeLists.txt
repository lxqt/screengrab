#cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${SG_INCLUDE})

message(STATUS "MKSPECS dir : " ${QT_MKSPECS_DIR})

set (extedit_SRC 
	moduleextedit.cpp
	extedit.cpp
    exteditconfig.cpp
	exteditconfigwidget.cpp 
)

set (extedit_HDR
    extedit.h
	exteditconfigwidget.h 
)

set(extedit_UI 
	exteditconfigwidget.ui 
)

qt4_wrap_cpp(extedit_SRC_MOC ${extedit_HDR})

qt4_wrap_ui(extedit_UI_H ${extedit_UI} )

if (Q_WS_WIN)
add_library(extedit STATIC ${extedit_SRC} ${extedit_SRC_MOC}  ${extedit_QRC} ${extedit_UI_H})
else (Q_WS_WIN)
add_library(extedit SHARED ${extedit_SRC} ${extedit_SRC_MOC}  ${extedit_QRC} ${extedit_UI_H})
set_property (TARGET extedit PROPERTY SOVERSION 1.0.0)
INSTALL (TARGETS extedit DESTINATION ${SG_LIBDIR})
endif (Q_WS_WIN)

target_link_libraries(extedit ${QT_LIBRARIES})
