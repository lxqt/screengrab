#cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${SG_INCLUDE})

message(STATUS "SG_CORE_INCLUDE-2: " ${SG_INCLUDE})

set (uploader_SRC 
	moduleuploader.cpp
	uploader.cpp
    imgshack/uploader_imgshack.cpp
	imgur/uploader_imgur.cpp
    uploaderconfig.cpp
    dialoguploader.cpp
)

set (uploader_HDR
    uploader.h
	imgshack/uploader_imgshack.h
	
	imgur/uploader_imgur.h
    dialoguploader.h
)

set(uploader_UI dialoguploader.ui)

qt4_wrap_cpp(uploader_SRC_MOC ${uploader_HDR})

qt4_wrap_ui(uploader_UI_H ${uploader_UI} )

add_library(uploader ${uploader_SRC} ${uploader_SRC_MOC}  ${uploader_QRC} ${uploader_UI_H})
target_link_libraries(uploader ${QT_LIBRARIES})
