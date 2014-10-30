#cmake_minimum_required(VERSION 2.8.11)

include_directories(${SG_INCLUDE})

set (extedit_SRC 
	moduleextedit.cpp
	extedit.cpp
)

set (extedit_HDR
    extedit.h
)

add_library(extedit SHARED ${extedit_SRC})
set_property (TARGET extedit PROPERTY SOVERSION 1.0.0)
INSTALL (TARGETS extedit DESTINATION ${SG_LIBDIR})

target_link_libraries(extedit Qt5::Widgets Qt5::X11Extras)
