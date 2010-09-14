#cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (singleapp_SRC 
  singleapp.cpp)

set (singleapp_HDR 
  singleapp.h
)

qt4_wrap_cpp(singleapp_SRC_MOC ${singleapp_HDR})

add_library(singleapp ${singleapp_SRC} ${singleapp_SRC_MOC}  ${singleapp_QRC})
target_link_libraries(singleapp ${QT_LIBRARIES})
