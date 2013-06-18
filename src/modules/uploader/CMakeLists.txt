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
	uploaderconfigwidget.cpp 
	imgur/uploader_imgur_widget.cpp 
	imgur/uploaderconfigwidget_imgur.cpp 
	imgshack/uploader_imgshack_widget.cpp 
	imgshack/uploaderconfigwidget_imgshack.cpp 
)

set (uploader_HDR
    moduleuploader.h 
    uploader.h
	imgshack/uploader_imgshack.h
	imgur/uploader_imgur.h
    dialoguploader.h
	uploaderconfigwidget.h 
	imgur/uploader_imgur_widget.h 
	imgur/uploaderconfigwidget_imgur.h 
	imgshack/uploader_imgshack_widget.h
	imgshack/uploaderconfigwidget_imgshack.h 
)

set(uploader_UI 
	dialoguploader.ui 
	uploaderconfigwidget.ui 
	imgur/uploader_imgur_widget.ui 
	imgur/uploaderconfigwidget_imgur.ui 
	imgshack/uploader_imgshack_widget.ui 
	imgshack/uploaderconfigwidget_imgshack.ui
)

qt4_wrap_cpp(uploader_SRC_MOC ${uploader_HDR})

qt4_wrap_ui(uploader_UI_H ${uploader_UI} )

if (Q_WS_WIN)
add_library(uploader STATIC ${uploader_SRC} ${uploader_SRC_MOC}  ${uploader_QRC} ${uploader_UI_H})
else (Q_WS_WIN)
add_library(uploader SHARED ${uploader_SRC} ${uploader_SRC_MOC}  ${uploader_QRC} ${uploader_UI_H})
set_property (TARGET uploader PROPERTY SOVERSION 1.0.0)
INSTALL (TARGETS uploader DESTINATION ${SG_LIBDIR})
endif (Q_WS_WIN)

target_link_libraries(uploader ${QT_LIBRARIES})
