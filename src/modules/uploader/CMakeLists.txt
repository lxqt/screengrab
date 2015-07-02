set(uploader_SRC
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

lxqt_translate_ts(uploader_QMS
    USE_QT5 TRUE
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    TEMPLATE "uploader"
    TRANSLATION_DIR "${CMAKE_SOURCE_DIR}/translations"
    SOURCES
        ${uploader_SRC}
        ${uploader_UI}
    INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations"
)

qt5_translation_loader(uploader_QM_LOADER
    "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/translations"
    "uploader"
)

add_library(uploader
    SHARED
        ${uploader_SRC}
        ${uploader_UI}
        ${uploader_QMS}
        ${uploader_QM_LOADER}
)

set_property (TARGET uploader PROPERTY SOVERSION 1.0.0)
install(TARGETS uploader DESTINATION ${SG_LIBDIR})

target_link_libraries(uploader Qt5::Widgets Qt5::Network Qt5::X11Extras)
