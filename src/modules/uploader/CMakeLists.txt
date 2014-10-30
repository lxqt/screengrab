#cmake_minimum_required(VERSION 2.8.11)

include_directories(${SG_INCLUDE})

set (uploader_SRC
	moduleuploader.cpp
	uploader.cpp
	imgur/uploader_imgur.cpp
    uploaderconfig.cpp
    dialoguploader.cpp
	uploaderconfigwidget.cpp
	imgur/uploader_imgur_widget.cpp
	imgur/uploaderconfigwidget_imgur.cpp
	mediacrush/uploader_mediacrush.cpp
	mediacrush/uploader_mediacrush_widget.cpp
	mediacrush/uploaderconfigwidget_mediacrush.cpp
)

set(uploader_UI
	dialoguploader.ui
	uploaderconfigwidget.ui
	imgur/uploader_imgur_widget.ui
	imgur/uploaderconfigwidget_imgur.ui
	mediacrush/uploader_mediacrush_widget.ui
	mediacrush/uploaderconfigwidget_mediacrush.ui
)

qt5_wrap_ui(uploader_UI_H ${uploader_UI})

add_library(uploader SHARED ${uploader_SRC} ${uploader_UI_H})
set_property (TARGET uploader PROPERTY SOVERSION 1.0.0)
INSTALL (TARGETS uploader DESTINATION ${SG_LIBDIR})

target_link_libraries(uploader Qt5::Widgets Qt5::Network Qt5::X11Extras)
