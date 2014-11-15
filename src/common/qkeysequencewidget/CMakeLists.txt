#cmake_minimum_required(VERSION 2.6)

add_definitions( -DIS_CMAKE="true")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS})

OPTION (QKSW_SHARED "Use QKeysequenseWidget as shared library" OFF)

set (QKSW_SRC 
  src/qkeysequencewidget.cpp)

set (QKSW_HDR 
  src/qkeysequencewidget.h
  src/qkeysequencewidget_p.h)

set (QKSW_QRC 
  ${CMAKE_CURRENT_SOURCE_DIR}/qkeysequencewidget.qrc)
  
qt5_add_resources(QKSW_QRC ${QKSW_QRC})
qt5_wrap_cpp(QKSW_SRC_MOC ${QKSW_HDR})

if(QKSW_SHARED)
	add_library(qkeysequencewidget SHARED ${QKSW_SRC} ${QKSW_SRC_MOC}  ${QKSW_QRC})
	add_definitions(-DIS_SHARED="true")
	set_property (TARGET qkeysequencewidget PROPERTY SOVERSION 1.0.0)
	INSTALL (TARGETS qkeysequencewidget DESTINATION ${SG_LIBDIR})
else(QKSW_SHARED)
	add_library(qkeysequencewidget STATIC ${QKSW_SRC} ${QKSW_SRC_MOC}  ${QKSW_QRC})
endif(QKSW_SHARED)

target_link_libraries(qkeysequencewidget Qt5::Widgets)
