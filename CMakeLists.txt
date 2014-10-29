cmake_minimum_required(VERSION 2.8.11)

# set project's name
project(screengrab)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# add version define
set(SCREENGRAB_VERSION "1.0.83")
set(SCREENGRAB_VERSION_DEV "1.1-dev")
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
    if(WIN32)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    endif(NOT MSVC)
endif(WIN32)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Woverloaded-virtual -Wall -Wextra")

add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})

if(UNIX AND NOT APPLE)
    # Check Debian or Ubuntu?
    if (EXISTS "/etc/debian_version")
        set (LIBDIR "lib/${CMAKE_LIBRARY_ARCHITECTURE}")
    # Other distro
    else()
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
endif()

# options
option(SG_GLOBALSHORTCUTS "Enable global shortcuts" OFF)
option(SG_XDG_CONFIG_SUPPORT "Put config files into XDGC_ONFIG_HOME" ON)
option(SG_EXT_UPLOADS "Enable upload screenshots to MediaCrush and imgur" ON)
option(SG_EXT_EDIT "Enable ability to edit screenshots in external editor" ON)

# The following line will add additional finders to CMake without the need to be placed in the CMake install path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


if(SG_GLOBALSHORTCUTS)
    add_definitions( -DSG_GLOBAL_SHORTCUTS="1")

    if(SG_USE_SYSTEM_QXT)
        # find qxt
        find_package(Qxt REQUIRED COMPONENTS QxtCore, QxtGui)
        include_directories(${QXT_QXTCORE_INCLUDE_DIR} ${QXT_QXTGUI_INCLUDE_DIR})
    endif()
endif(SG_GLOBALSHORTCUTS)

if(SG_XDG_CONFIG_SUPPORT)
    add_definitions( -DSG_XDG_CONFIG_SUPPORT="1")
endif()

if(SG_EXT_UPLOADS)
    add_definitions( -DSG_EXT_UPLOADS="1")
endif()

if(SG_EXT_EDIT)
    add_definitions( -DSG_EXT_EDIT="1")
endif()

message(STATUS "Global shortcuts support: " ${SG_GLOBALSHORTCUTS})
message(STATUS "XDG_CONFIG_HOME support: " ${SG_XDG_CONFIG_SUPPORT})
message(STATUS "Upload to MediaCrush and imgur support: " ${SG_EXT_UPLOADS})
message(STATUS "Editing screenshots in external editor support: " ${SG_EXT_EDIT})
message(STATUS "Use system Qxt Library: " ${SG_USE_SYSTEM_QXT})

# docs
if(UNIX AND NOT APPLE)
    if(NOT SG_DOCDIR)
         set(SG_DOCDIR "screengrab" CACHE STRING "Documentation directory name")
    endif()
    set(DOCPATH "share/doc/${SG_DOCDIR}" CACHE STRING "Documentation directory name")
    message(STATUS "Documentation directory: " ${CMAKE_INSTALL_PREFIX}/${DOCPATH})
    add_definitions(-DSG_DOCDIR="${CMAKE_INSTALL_PREFIX}/${DOCPATH}")
endif()

set(SG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# qtsingleapplication
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

#openarenainclude_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
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
    src/main.cpp
    src/core/core.cpp
    src/core/config.cpp
    src/core/regionselect.cpp
    src/core/cmdline.cpp
    src/core/shortcutmanager.cpp
    src/core/modulemanager.cpp
    src/ui/configwidget.cpp
    src/ui/about.cpp
    src/ui/mainwindow.cpp
)

if(UNIX AND NOT APPLE)
    set(SCREENGRAB_SRC ${SCREENGRAB_SRC} src/common/netwm/netwm.cpp)
endif()

if(WIN32)
    set(WIN_RC screengrab.rc)
endif()

# set headers to moc
set(SCREENGRAB_MOC
    src/core/core.h
    src/core/regionselect.h
    src/ui/configwidget.h
    src/ui/about.h
    src/ui/mainwindow.h
)

# set headers to moc
set(SCREENGRAB_UI
    src/ui/configwidget.ui
    src/ui/aboutwidget.ui
    src/ui/mainwindow.ui
)

# documentation files
set(DOCS
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/ChangeLog.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/LICENSE.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
)

# Qt resource file
set(SCREENGRAB_QRC
    screengrab.qrc
)

qt5_wrap_ui(SCREENGRAB_UI_H ${SCREENGRAB_UI})
qt5_add_resources(QRC_SOURCES ${SCREENGRAB_QRC})

# find lrelease binary
if(NOT QT_LRELEASE_EXECUTABLE)
    find_program(QT_LRELEASE_EXECUTABLE
        NAMES lrelease
        PATHS ${QT_BINARY_DIR}
        NO_DEFAULT_PATH
    )
endif()

# debian hack
if(EXISTS "${QT_BINARY_DIR}/lrelease-qt4")
    set(QT_LRELEASE_EXECUTABLE "${QT_BINARY_DIR}/lrelease-qt4")
endif()

# if(QT_LRELEASE_EXECUTABLE)
#     message(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
#     message(STATUS "Generating localize ...")
#     execute_process(COMMAND find "${CMAKE_CURRENT_SOURCE_DIR}/localize" -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)
# else()
#     message(FATAL_ERROR "Could NOT find lrelease executable")
# endif()

if(WIN32 AND NOT MSVC)
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o
    COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/screengrab.rc -o "${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o")
    set(SCREENGRAB_SRC ${SCREENGRAB_SRC} "${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o")
endif(WIN32 AND NOT MSVC)


add_executable( screengrab ${SCREENGRAB_SRC} ${SCREENGRAB_UI_H} ${QRC_SOURCES} )

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

if(WIN32)
    target_link_libraries(screengrab qkeysequencewidget singleapp ${QT_LIBRARIES})
endif()

if(UNIX)
    target_link_libraries(screengrab qkeysequencewidget singleapp X11 Qt5::Widgets)
endif()

if(WIN32 AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows ")
endif()

if(UNIX AND NOT APPLE)
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
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/screengrab.desktop" DESTINATION share/applications)

    # install pixmap
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/screengrab.png" DESTINATION share/pixmaps)

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/localize/" DESTINATION share/screengrab/localize FILES_MATCHING PATTERN "*.qm")
endif()

if(WIN32)
    set(WIN32_BINARY_RUNTIME "${CMAKE_FIND_ROOT_PATH}/bin/zlib1.dll")

    set(WIN32_BINARY_QT_LIBS
        "${QT_BINARY_DIR}/QtCore4.dll"
        "${QT_BINARY_DIR}/QtGui4.dll"
        "${QT_BINARY_DIR}/QtNetwork4.dll"
    )

    install(TARGETS screengrab RUNTIME DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro")
    install(FILES ${WIN32_BINARY_RUNTIME} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro")
    install(FILES ${WIN32_BINARY_QT_LIBS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro")
    install(FILES ${QT_JPEG_LIBRARY} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin/imageformats")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/localize/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro/localize" FILES_MATCHING PATTERN "*.qm")
    install(FILES ${DOCS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro/docs")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/distro/docs/html")
endif()
