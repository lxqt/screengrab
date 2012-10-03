cmake_minimum_required( VERSION 2.6 )

# set project's name
project( screengrab )

message(STATUS "System: " ${CMAKE_HOST_SYSTEM_NAME} " " ${CMAKE_HOST_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})

find_package( Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
include( "${QT_USE_FILE}" )

# add version define
set(SCREENGRAB_VERSION "0.9.96")
set(SCREENGRAB_VERSION_DEV "1.0-rc")
# set(DEV_BUILD True)

if(SCREENGRAB_VERSION_DEV)
if (DEV_BUILD)
# search git executable
find_package(Git)
if (GIT_FOUND)

    # executable git for check HEAD rev hash
    execute_process (COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(GIT_FOUND)

    set(VERSION "${SCREENGRAB_VERSION}  (${SCREENGRAB_VERSION_DEV}-git-${GIT_HASH})")
else (DEV_BUILD)
    set(VERSION "${SCREENGRAB_VERSION}  (${SCREENGRAB_VERSION_DEV})")
endif(DEV_BUILD)
elseif(NOT SCREENGRAB_VERSION_DEV)
  set(VERSION ${SCREENGRAB_VERSION})
endif(SCREENGRAB_VERSION_DEV)

add_definitions( -DVERSION="${VERSION}")
message(STATUS "Version				: " ${VERSION})
#${CMAKE_SYSTEM_NAME} [${CMAKE_HOST_SYSTEM_PROCESSOR}]" )

message(STATUS "Use qmake: " ${QT_QMAKE_EXECUTABLE})
# ------ build type ------------------------

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_definitions(-DNDEBUG)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
    if(WIN32)
    if(NOT MSVC)	
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    endif(NOT MSVC)
endif(WIN32)
endif(${CMAKE_BUILD_TYPE} MATCHES "Release")
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "-Wnon-virtual-dtor -Woverloaded-virtual -Wall -Wextra")

# ------ install prefix --------------------------

set(CMAKE_INSTALL_PREFIX "/usr")

add_definitions( -DPREFIX="${CMAKE_INSTALL_PREFIX}")

message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})

# ------ options ---------------------------------
OPTION (SG_GLOBALSHORTCUTS "Enable global shortcuts" ON)
OPTION (SG_XDG_CONFIG_SUPPORT "Place config files into XDGC_ONFIG_HOME" ON)
OPTION (SG_EXT_UPLOADS "Enableupload screnshots to image hostings" ON)
OPTION (SG_USE_SYSTEM_QXT "Use system version Qxt Library for global shortcuts" OFF)

# The following line will add additional finders to CMake without the need to be placed in the CMake install path
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


if(SG_GLOBALSHORTCUTS)
    add_definitions( -DSG_GLOBAL_SHORTCUTS="1")
    
	if(SG_USE_SYSTEM_QXT)
		# find qxt
		find_package(Qxt REQUIRED COMPONENTS QxtCore, QxtGui)
		include_directories(${QXT_QXTCORE_INCLUDE_DIR} ${QXT_QXTGUI_INCLUDE_DIR})
	else(SG_USE_SYSTEM_QXT)
		include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxt/include/")    
		add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxt/")
		# build QxtGui
		add_definitions(-DBUILD_QXT_GUI)
	endif(SG_USE_SYSTEM_QXT)
endif(SG_GLOBALSHORTCUTS)

if(SG_XDG_CONFIG_SUPPORT)
    add_definitions( -DSG_XDG_CONFIG_SUPPORT="1")
endif(SG_XDG_CONFIG_SUPPORT)

if(SG_EXT_UPLOADS)
    add_definitions( -DSG_EXT_UPLOADS="1")
endif(SG_EXT_UPLOADS)

message(STATUS "Global shortcuts support: " ${SG_GLOBALSHORTCUTS})
message(STATUS "XDG_CONFIG_HOME support: " ${SG_XDG_CONFIG_SUPPORT})
message(STATUS "Upload to image hosings support: " ${SG_EXT_UPLOADS})
message(STATUS "Use system Qxt Library: " ${SG_USE_SYSTEM_QXT})

# ------ doc dir name ---------------------------------
if(UNIX AND NOT APPLE)
    if (NOT SG_DOCDIR)
         set(SG_DOCDIR "screengrab" CACHE STRING "Documantation directory name")        
    endif (NOT SG_DOCDIR)
    set(DOCPATH "share/doc/${SG_DOCDIR}" CACHE STRING "Documantation directory name")
    message(STATUS "Documentation directory: " ${CMAKE_INSTALL_PREFIX}/${DOCPATH})
    add_definitions( -DSG_DOCDIR="${CMAKE_INSTALL_PREFIX}/${DOCPATH}")
endif(UNIX AND NOT APPLE)

set(SG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
message(STATUS "SG_INCLUDE: " ${SG_INCLUDE})

#------- qtsingleapplication  --------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

#openarenainclude_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

if(SG_EXT_UPLOADS)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")
endif(SG_EXT_UPLOADS)

#------- sources --------

# with SET() command you can change variables or define new ones
# here we define SAMPLE_SRCS variable that contains a list of all .cpp files
# note that we don't need \ at the end of line
set(SCREENGRAB_SRC
    src/main.cpp
    src/core/core.cpp
    src/core/config.cpp
    src/core/regionselect.cpp
    src/core/cmdline.cpp
    src/core/shortcutmanager.cpp
    src/ui/configwidget.cpp
    src/ui/about.cpp
    src/ui/mainwindow.cpp
)

if(UNIX AND NOT APPLE)
 set(SCREENGRAB_SRC ${SCREENGRAB_SRC}  src/common/netwm/netwm.cpp)
endif(UNIX AND NOT APPLE)

if(WIN32)
    set(WIN_RC screengrab.rc)
endif(WIN32)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/docs/ChangeLog.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/docs/LICENSE.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/README.md
)

#  include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)

# Qt resource file
set( SCREENGRAB_QRC screengrab.qrc )


include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")


  # this will run uic on .ui files:
qt4_wrap_ui(SCREENGRAB_UI_H ${SCREENGRAB_UI} )

# genereate moc headers
qt4_wrap_cpp(SCREENGRAB_MOC_CPP ${SCREENGRAB_MOC} )

# genereating resource cpp
qt4_add_resources( QRC_SOURCES ${SCREENGRAB_QRC} )


#------- find lrelease & create qms ------------

# find lrelease binary
if(NOT QT_LRELEASE_EXECUTABLE)
find_program(QT_LRELEASE_EXECUTABLE
    NAMES lrelease
    PATHS ${QT_BINARY_DIR}
    NO_DEFAULT_PATH
)
endif(NOT QT_LRELEASE_EXECUTABLE)

# debian hack
if(EXISTS ${QT_BINARY_DIR}/lrelease-qt4)
set(QT_LRELEASE_EXECUTABLE ${QT_BINARY_DIR}/lrelease-qt4)
endif(EXISTS ${QT_BINARY_DIR}/lrelease-qt4)

if(QT_LRELEASE_EXECUTABLE)
message(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
message(STATUS "Generating localize ...")
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/localize -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)
else(QT_LRELEASE_EXECUTABLE)
message(FATAL_ERROR "Could NOT find lrelease executable")
endif(QT_LRELEASE_EXECUTABLE)

# --exclude-backups --exclude-vcs --exclude=localize/*qm --exclude=*.kdev4 --exclude=build --exclude=create-src.sh --exclude=win32
#------- add icon & trmove console in win32 (mingw) ----------

if(WIN32)
    if(NOT MSVC)
        add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o
        COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/screengrab.rc
        -o ${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o )

        set(SCREENGRAB_SRC ${SCREENGRAB_SRC} ${CMAKE_CURRENT_BINARY_DIR}/screengrabrc.o)
    endif(NOT MSVC)
endif(WIN32)

#-------  build executable from sources ----------
add_executable( screengrab ${SCREENGRAB_SRC} ${SCREENGRAB_MOC_CPP} ${SCREENGRAB_UI_H} ${QRC_SOURCES} )

if(SG_GLOBALSHORTCUTS)
	if(SG_USE_SYSTEM_QXT)
		target_link_libraries( screengrab ${QXT_QXTCORE_LIB_RELEASE} ${QXT_QXTGUI_LIB_RELEASE})
	else(SG_USE_SYSTEM_QXT)
		target_link_libraries( screengrab qxt)
    endif(SG_USE_SYSTEM_QXT)
endif(SG_GLOBALSHORTCUTS)

if(SG_EXT_UPLOADS)
	target_link_libraries( screengrab uploader)
endif(SG_EXT_UPLOADS)

if(WIN32)
    target_link_libraries( screengrab qkeysequencewidget singleapp ${QT_LIBRARIES} )
endif(WIN32)
if(UNIX)
    target_link_libraries( screengrab qkeysequencewidget singleapp X11 
${QT_LIBRARIES} )
endif(UNIX)

if(WIN32)
    if(NOT MSVC)

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows ")

    endif(NOT MSVC)
endif(WIN32)

#-------  make src packae and installing ----------

if(UNIX AND NOT APPLE)
    # make src.tar.gz
    ADD_CUSTOM_TARGET (dist @echo create source package )
    set(SCREENGRAB_DIST "screengrab-${SCREENGRAB_VERSION}")
    add_custom_command(COMMAND cp 
        ARGS -R ${CMAKE_CURRENT_SOURCE_DIR} "/tmp/${SCREENGRAB_DIST}" 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        TARGET  dist
    )

    ADD_CUSTOM_COMMAND(COMMAND tar 
        ARGS cvfz ${CMAKE_CURRENT_BINARY_DIR}/${SCREENGRAB_DIST}.tar.gz --exclude=*~ --exclude-vcs --exclude=localize/*qm --exclude=*.kdev4 --exclude=build --exclude=create-src.sh --exclude=win32 -C "/tmp" "${SCREENGRAB_DIST}"  
        WORKING_DIRECTORY "/tmp"
        TARGET  dist
    )

    add_custom_command(COMMAND rm 
        ARGS -rf "/tmp/${SCREENGRAB_DIST}"
        WORKING_DIRECTORY "/tmp"
        TARGET  dist
    )

    # installing
    INSTALL(TARGETS screengrab RUNTIME DESTINATION bin)

    # install txt docs
    INSTALL(FILES ${DOCS} DESTINATION ${DOCPATH})
    # install html docs
    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html/" DESTINATION "${DOCPATH}/html")

    # install desktop files
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/screengrab.desktop" DESTINATION share/applications)

    # install pixmap
    INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/screengrab.png" DESTINATION share/pixmaps)

    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/localize/" DESTINATION share/screengrab/localize FILES_MATCHING PATTERN "*.qm")
endif(UNIX AND NOT APPLE)

if(WIN32)
    set(WIN32_BINARY_RUNTIME 
#         ${CMAKE_FIND_ROOT_PATH}/bin/libgcc_s_sjlj-1.dll
#         ${CMAKE_FIND_ROOT_PATH}/bin/libstdc++-6.dll
        ${CMAKE_FIND_ROOT_PATH}/bin/zlib1.dll
    )

    set(WIN32_BINARY_QT_LIBS 
        ${QT_BINARY_DIR}/QtCore4.dll
        ${QT_BINARY_DIR}/QtGui4.dll
        ${QT_BINARY_DIR}/QtNetwork4.dll
    )
    
    INSTALL(TARGETS screengrab RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro)
    install(FILES ${WIN32_BINARY_RUNTIME} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro) 
    install(FILES ${WIN32_BINARY_QT_LIBS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro)
    install(FILES ${QT_JPEG_LIBRARY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/imageformats)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/localize/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro/localize FILES_MATCHING PATTERN "*.qm")
    install(FILES ${DOCS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro/docs)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/distro/docs/html)

endif(WIN32)