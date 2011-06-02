cmake_minimum_required( VERSION 2.6 )

# set project's name
project( screengrab )

message(STATUS "System: " ${CMAKE_HOST_SYSTEM_NAME} " " ${CMAKE_HOST_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})

find_package( Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
include( "${QT_USE_FILE}" )

# add version define
set(SCREENGRAB_VERSION "0.9.81")
set(SCREENGRAB_VERSION_DEV "1.0")

if(SCREENGRAB_VERSION_DEV)

# search git executable
find_package(Git)
if (GIT_FOUND)

    # executable git for check HEAD rev hash
    execute_process (COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(GIT_FOUND)

    set(VERSION "${SCREENGRAB_VERSION}  (${SCREENGRAB_VERSION_DEV}-git-${GIT_HASH})")
elseif(NOT SCREENGRAB_VERSION_DEV)
  set(VERSION ${SCREENGRAB_VERSION})
endif(SCREENGRAB_VERSION_DEV)

add_definitions( -DVERSION="${VERSION}")
message(STATUS "Version: " ${VERSION})
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

set(CMAKE_CXX_FLAGS "-Wnon-virtual-dtor -Woverloaded-virtual -Wall")
# ------ options ---------------------------------
OPTION (SG_GLOBALSHORTCUTS "Enable global shortcuts" ON)

if(SG_GLOBALSHORTCUTS)
    add_definitions( -DSG_GLOBAL_SHORTCUTS="1")
endif(SG_GLOBALSHORTCUTS)

message(STATUS "Global shortcuts support: " ${SG_GLOBALSHORTCUTS})
# ------ install prefix --------------------------

set(CMAKE_INSTALL_PREFIX "/usr")

add_definitions( -DPREFIX="${CMAKE_INSTALL_PREFIX}")

message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})

set(SG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
message(STATUS "SG_INCLUDE: " ${SG_INCLUDE})

#------- qtsingleapplication  --------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/modules/uploader/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/singleapp")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/common/qkeysequencewidget/src")

if(SG_GLOBALSHORTCUTS)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxt/include/")    
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/qxt/")
endif(SG_GLOBALSHORTCUTS)

# build QxtGui
add_definitions(-DBUILD_QXT_GUI)
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
  ${CMAKE_CURRENT_SOURCE_DIR}/docs/README.txt
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
    target_link_libraries( screengrab qxt)
endif(SG_GLOBALSHORTCUTS)

if(WIN32)
    target_link_libraries( screengrab qkeysequencewidget singleapp uploader ${QT_LIBRARIES} )
endif(WIN32)
if(UNIX)
    target_link_libraries( screengrab qkeysequencewidget singleapp uploader  X11 
${QT_LIBRARIES} )
endif(UNIX)

if(WIN32)
    if(NOT MSVC)

        set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-mwindows")

    endif(NOT MSVC)
endif(WIN32)

#installs unix
if(UNIX AND NOT APPLE)
#INSTALL_TARGETS(/bin screengrab)
  INSTALL(TARGETS screengrab RUNTIME DESTINATION bin)

  # install txt docs
  INSTALL(FILES ${DOCS} DESTINATION share/doc/screengrab)
  # install html docs
  INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/html/" DESTINATION share/doc/screengrab/html)

  # install desktop files
  INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/screengrab.desktop" DESTINATION share/applications)

  # install pixmap
  INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/img/screengrab.png" DESTINATION share/pixmaps)

  # istall localize
#  FILE(GLOB qm "${CMAKE_CURRENT_SOURCE_DIR}/localize/*.qm")
#  INSTALL(FILES ${qm} DESTINATION share/screengrab/localize)
  INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/localize/" DESTINATION share/screengrab/localize FILES_MATCHING PATTERN "*.qm")
endif(UNIX AND NOT APPLE)
