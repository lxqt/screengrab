cmake_minimum_required(VERSION 2.8.11)

# set project's name
project(screengrab)

find_package(Qt5Widgets 5.2.0 REQUIRED)
find_package(Qt5X11Extras 5.2.0 REQUIRED)
find_package(Qt5Network 5.2.0 REQUIRED)
find_package(KF5WindowSystem REQUIRED)

# for translations
find_package(Qt5LinguistTools REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# add version define
set(SCREENGRAB_VERSION "1.91")
set(SCREENGRAB_VERSION_DEV "2.0-dev")
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

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_definitions(-DNDEBUG)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Woverloaded-virtual -Wall -Wextra")

message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})

if(DEFINED LIB_SUFFIX)
    set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "Library Installation Suffix")
    message(STATUS "Library Install Suffix: " ${LIB_SUFFIX})
    set(LIBDIR "lib/${LIB_SUFFIX}" CACHE STRING "Library Installation Dir")
else()
    # find libs dir
    string (COMPARE EQUAL "${CMAKE_SIZEOF_VOID_P}" "8" IS64BITS)
    if(IS64BITS)
        set(LIBDIR "lib64")
    else()
        set(LIBDIR "lib")
    endif()
endif()

set(SG_LIBDIR "${LIBDIR}/screengrab")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${SG_LIBDIR}")
message(STATUS "Library path: ${CMAKE_INSTALL_RPATH}")

# options
option(SG_GLOBALSHORTCUTS "Enable global shortcuts" OFF)
option(SG_EXT_UPLOADS "Enable upload screenshots to MediaCrush and imgur" ON)
option(SG_EXT_EDIT "Enable ability to edit screenshots in external editor" ON)
option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# The following line will add additional finders to CMake without the need to be placed in the CMake install path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
    include(${QTXDG_USE_FILE})
endif()

message(STATUS "Global shortcuts support: " ${SG_GLOBALSHORTCUTS})
message(STATUS "Upload to MediaCrush and imgur support: " ${SG_EXT_UPLOADS})
message(STATUS "Editing screenshots in external editor support: " ${SG_EXT_EDIT})
message(STATUS "Use system Qxt Library: " ${SG_USE_SYSTEM_QXT})
message(STATUS "Update source translation translations/*.ts files: " ${UPDATE_TRANSLATIONS})

# docs
if(NOT SG_DOCDIR)
    set(SG_DOCDIR "screengrab" CACHE STRING "Documentation directory name")
endif()
set(DOCPATH "share/doc/${SG_DOCDIR}" CACHE STRING "Documentation directory name")
message(STATUS "Documentation directory: " ${CMAKE_INSTALL_PREFIX}/${DOCPATH})
add_definitions(-DSG_DOCDIR="${CMAKE_INSTALL_PREFIX}/${DOCPATH}")

set(SG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/")

include_directories("${SG_INCLUDE}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

if(SG_EXT_UPLOADS)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")
endif()

if (SG_EXT_EDIT)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/extedit/")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/extedit/")
endif()

set(SCREENGRAB_SRC
    src/core/main.cpp
    src/core/singleapp.cpp
    src/core/core.cpp
    src/core/config.cpp
    src/core/regionselect.cpp
    src/core/cmdline.cpp
    src/core/shortcutmanager.cpp
    src/core/modulemanager.cpp
    src/core/ui/configwidget.cpp
    src/core/ui/about.cpp
    src/core/ui/mainwindow.cpp
)

set(SCREENGRAB_HDR
    src/core/singleapp.h
)

set(SCREENGRAB_UI
    src/core/ui/configwidget.ui
    src/core/ui/aboutwidget.ui
    src/core/ui/mainwindow.ui
)

# Qt resource file
set(SCREENGRAB_QRC
    screengrab.qrc
)

qt5_wrap_ui(SCREENGRAB_UI_H ${SCREENGRAB_UI})
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
    INSTALL_DIR "share/${PROJECT_NAME}/translations"
)

lxqt_translate_desktop(SCREENGRAB_DESKTOP_FILES
    SOURCES ${SCREENGRAB_DESKTOP_FILES_IN}
)

qt5_translation_loader(SCREENGRAB_QM_LOADER
    "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/translations"
    ${PROJECT_NAME}
)

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

target_link_libraries(screengrab qkeysequencewidget Qt5::Widgets KF5::WindowSystem)


# make src.tar.gz
add_custom_target(dist @echo create source package)
set(SCREENGRAB_DIST "screengrab-${SCREENGRAB_VERSION}")
add_custom_command(COMMAND cp
    ARGS -R ${CMAKE_CURRENT_SOURCE_DIR} "/tmp/${SCREENGRAB_DIST}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    TARGET dist
)

ADD_CUSTOM_COMMAND(COMMAND tar
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
# install txt docs
install(FILES ${DOCS} DESTINATION ${DOCPATH})
# install html docs
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html/" DESTINATION "${DOCPATH}/html")
# install desktop files
install(FILES ${SCREENGRAB_DESKTOP_FILES} DESTINATION share/applications)
# install pixmap
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/screengrab.png" DESTINATION share/pixmaps)
