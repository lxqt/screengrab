cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

# set project's name
project(screengrab)

find_package(Qt5Widgets 5.2.0 REQUIRED)
find_package(Qt5X11Extras 5.2.0 REQUIRED)
find_package(Qt5Network 5.2.0 REQUIRED)
find_package(KF5WindowSystem REQUIRED)

# for translations
find_package(Qt5LinguistTools REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# long live cmake + qt :)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(X11)
if (X11_FOUND)
    set(HAVE_X11 1)
endif(X11_FOUND)

# set up xcb and x11_xcb

find_package(
    XCB MODULE COMPONENTS
        XCB
        SHAPE
        XFIXES
)

find_package(
    X11_XCB MODULE
)

# add version define
set(SCREENGRAB_VERSION "1.95")
set(SCREENGRAB_VERSION_DEV "2.0-beta1")
# set(DEV_BUILD True)

if(SCREENGRAB_VERSION_DEV)
    if(DEV_BUILD)
        # search git executable
        find_package(Git)
        if(GIT_FOUND)
        # executable git for check HEAD rev hash
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        endif()

        set(VERSION "${SCREENGRAB_VERSION}  (${SCREENGRAB_VERSION_DEV}-git-${GIT_HASH})")
    else()
        set(VERSION "${SCREENGRAB_VERSION}  (${SCREENGRAB_VERSION_DEV})")
    endif()
elseif(NOT SCREENGRAB_VERSION_DEV)
    set(VERSION "${SCREENGRAB_VERSION}")
endif(SCREENGRAB_VERSION_DEV)

add_definitions(-DVERSION="${VERSION}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Woverloaded-virtual -Wall -Wextra")

set(SG_LIBDIR "${CMAKE_INSTALL_LIBDIR}/screengrab")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${SG_LIBDIR}")
message(STATUS "Library path: ${CMAKE_INSTALL_RPATH}")

# options
option(SG_GLOBALSHORTCUTS "Enable global shortcuts" OFF)
option(SG_EXT_UPLOADS "Enable upload screenshots to Imgur" ON)
option(SG_EXT_EDIT "Enable ability to edit screenshots in external editor" ON)
option(SG_DBUS_NOTIFY "Enable D-Bus notifications" ON)
option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Although the names, LXQtTranslateTs and LXQtTranslateDesktop, they don't
#   bring any dependency on lxqt.
include(LXQtTranslateTs)
include(LXQtTranslateDesktop)
include(Qt5TranslationLoader)

if(SG_GLOBALSHORTCUTS)
    add_definitions( -DSG_GLOBAL_SHORTCUTS="1")

    if(SG_USE_SYSTEM_QXT)
        # find qxt
        find_package(Qxt REQUIRED COMPONENTS QxtCore, QxtGui)
        include_directories(${QXT_QXTCORE_INCLUDE_DIR} ${QXT_QXTGUI_INCLUDE_DIR})
    endif()
endif(SG_GLOBALSHORTCUTS)

if(SG_EXT_UPLOADS)
    add_definitions( -DSG_EXT_UPLOADS="1")
endif()

if(SG_EXT_EDIT)
    add_definitions( -DSG_EXT_EDIT="1")
    find_package(Qt5Xdg REQUIRED)
endif()

if(SG_DBUS_NOTIFY)
    find_package(Qt5DBus 5.2 REQUIRED)
    add_definitions( -DSG_DBUS_NOTIFY="1")
endif()

message(STATUS "Global shortcuts support: " ${SG_GLOBALSHORTCUTS})
message(STATUS "Imgur upload support: " ${SG_EXT_UPLOADS})
message(STATUS "Editing screenshots in external editor support: " ${SG_EXT_EDIT})
message(STATUS "Enable D-Bus notifications: " ${SG_DBUS_NOTIFY})
message(STATUS "Use system Qxt Library: " ${SG_USE_SYSTEM_QXT})
message(STATUS "Update source translation translations/*.ts files: " ${UPDATE_TRANSLATIONS})

# docs
# CMAKE_INSTALL_FULL_DOCDIR = CMAKE_INSTALL_PREFIX/CMAKE_INSTALL_DATADIR/doc/PROJECT_NAME
message(STATUS "Documentation directory: " ${CMAKE_INSTALL_FULL_DOCDIR})
add_definitions(-DSG_DOCDIR="${CMAKE_INSTALL_FULL_DOCDIR}")

# app icon path
add_definitions(-DSG_ICONPATH="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/pixmaps/screengrab.png")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

if(SG_EXT_UPLOADS)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader")
endif()

if (SG_EXT_EDIT)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/extedit")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/extedit")
endif()

set(SCREENGRAB_SRC
    src/core/main.cpp
    src/core/singleapp.cpp
    src/core/core.cpp
    src/core/config.cpp
    src/core/regionselect.cpp
    src/core/shortcutmanager.cpp
    src/core/modulemanager.cpp
    src/core/ui/configwidget.cpp
    src/core/ui/about.cpp
    src/core/ui/mainwindow.cpp
)

if(SG_DBUS_NOTIFY)
    set(SCREENGRAB_SRC ${SCREENGRAB_SRC} src/core/dbusnotifier.cpp)
endif()

if(X11_FOUND)
    set(SCREENGRAB_SRC ${SCREENGRAB_SRC} src/core/x11utils.cpp)
endif()

set(SCREENGRAB_HDR
    src/core/singleapp.h
)

set(SCREENGRAB_UI
    src/core/ui/configwidget.ui
    src/core/ui/aboutwidget.ui
    src/core/ui/mainwindow.ui
)

# Qt resource file
set(SCREENGRAB_QRC screengrab.qrc)
qt5_add_resources(QRC_SOURCES ${SCREENGRAB_QRC})

message(STATUS "Generating localize ...")

set(SCREENGRAB_DESKTOP_FILES_IN
    screengrab.desktop.in
)

lxqt_translate_ts(SCREENGRAB_QMS
    USE_QT5 TRUE
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    SOURCES
        ${SCREENGRAB_SRC}
        ${SCREENGRAB_UI}
    INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations"
)

lxqt_translate_desktop(SCREENGRAB_DESKTOP_FILES
    SOURCES ${SCREENGRAB_DESKTOP_FILES_IN}
)

qt5_translation_loader(SCREENGRAB_QM_LOADER
    "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/translations"
    ${PROJECT_NAME}
)

# generating executable
add_executable(screengrab
    ${SCREENGRAB_SRC}
    ${SCREENGRAB_UI_H}
    ${QRC_SOURCES}
    ${SCREENGRAB_QMS}
    ${SCREENGRAB_DESKTOP_FILES}
    ${SCREENGRAB_QM_LOADER}
)

if(SG_GLOBALSHORTCUTS)
    if(SG_USE_SYSTEM_QXT)
        target_link_libraries(screengrab ${QXT_QXTCORE_LIB_RELEASE} ${QXT_QXTGUI_LIB_RELEASE})
    else(SG_USE_SYSTEM_QXT)
        target_link_libraries(screengrab qxt)
    endif(SG_USE_SYSTEM_QXT)
endif()

if(SG_EXT_UPLOADS)
    target_link_libraries(screengrab uploader)
endif()

if(SG_EXT_EDIT)
    target_link_libraries(screengrab extedit)
endif()

if(SG_DBUS_NOTIFY)
    target_link_libraries(screengrab Qt5::DBus)
endif()

if (X11_XCB_FOUND)
    add_definitions( -DX11_XCB_FOUND="1")
    target_link_libraries(screengrab ${X11_XCB_LIBRARIES})
endif()

if (XCB_XCB_FOUND)
    add_definitions( -DXCB_XCB_FOUND="1")
    target_link_libraries(screengrab ${XCB_XCB_LIBRARY})
endif()

if (XCB_SHAPE_FOUND)
    add_definitions( -DXCB_SHAPE_FOUND="1")
    target_link_libraries(screengrab ${XCB_SHAPE_LIBRARY})
endif()

if (XCB_XFIXES_FOUND)
    add_definitions( -DXCB_XFOXES_FOUND="1")
    target_link_libraries(screengrab ${XCB_XFIXES_LIBRARY})
endif()

target_link_libraries(screengrab qkeysequencewidget Qt5::Widgets KF5::WindowSystem ${X11_LIBRARIES})

# make src.tar.gz
add_custom_target(dist @echo create source package)
set(SCREENGRAB_DIST "screengrab-${SCREENGRAB_VERSION}")
add_custom_command(COMMAND cp
    ARGS -R ${CMAKE_CURRENT_SOURCE_DIR} "/tmp/${SCREENGRAB_DIST}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    TARGET dist
)

add_custom_command(COMMAND tar
    ARGS cvfz "${CMAKE_CURRENT_BINARY_DIR}/${SCREENGRAB_DIST}.tar.gz" --exclude=*~ --exclude-vcs --exclude=localize/*qm --exclude=*.kdev4 --exclude=build --exclude=create-src.sh --exclude=win32 -C "/tmp" "${SCREENGRAB_DIST}"
    WORKING_DIRECTORY "/tmp"
    TARGET dist
)

add_custom_command(COMMAND rm
    ARGS -rf "/tmp/${SCREENGRAB_DIST}"
    WORKING_DIRECTORY "/tmp"
    TARGET dist
)

# installing
install(TARGETS screengrab RUNTIME DESTINATION bin)
# install html docs
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html" DESTINATION "${CMAKE_INSTALL_FULL_DOCDIR}")
# install desktop files
install(FILES ${SCREENGRAB_DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
# install pixmap
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/screengrab.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps")
