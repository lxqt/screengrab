set (extedit_SRC
    moduleextedit.cpp
    extedit.cpp
)

set (extedit_HDR
    extedit.h
)

lxqt_translate_ts(extedit_QMS
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    TEMPLATE "extedit"
    TRANSLATION_DIR "${CMAKE_SOURCE_DIR}/translations"
    SOURCES
        ${extedit_SRC}
        ${extedit_HDR}
    INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations"
)

qt6_translation_loader(extedit_QM_LOADER
    "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/translations"
    "extedit"
)

add_library(extedit
    STATIC
        ${extedit_SRC}
        ${extedit_QMS}
        ${extedit_QM_LOADER}
)

target_link_libraries(extedit Qt6::Widgets Qt6Xdg)
