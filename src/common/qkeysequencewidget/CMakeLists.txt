include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS})

set (QKSW_SRC
  src/qkeysequencewidget.cpp)

set (QKSW_HDR
  src/qkeysequencewidget.h
  src/qkeysequencewidget_p.h)

set (QKSW_QRC
  qkeysequencewidget.qrc)

qt5_add_resources(QKSW_QRC ${QKSW_QRC})

add_library(qkeysequencewidget STATIC ${QKSW_SRC} ${QKSW_QRC})

target_link_libraries(qkeysequencewidget Qt5::Widgets)
