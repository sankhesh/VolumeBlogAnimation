cmake_minimum_required(VERSION 2.8)

PROJECT(VolumeBlogAnimation)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(SRCS
  VolumeBlogAnimation.cxx
  pugixml/pugixml.cpp
  pugixml/pugixml.hpp
  pugixml/pugiconfig.hpp
  )

add_executable(VolumeBlogAnimation MACOSX_BUNDLE
  ${SRCS})

if(VTK_LIBRARIES)
  target_link_libraries(VolumeBlogAnimation ${VTK_LIBRARIES})
else()
  target_link_libraries(VolumeBlogAnimation vtkHybrid vtkWidgets)
endif()
#
#add_custom_command(
#  OUTPUT ${CMAKE_BINARY_DIR}/Data
#  COMMAND ${CMAKE_COMMAND}
#  ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/Data ${CMAKE_BINARY_DIR}/Data
#  COMMENT "Copying Data directory"
#  MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/Data
#  )
#add_custom_target(copy_data ALL DEPENDS "${CMAKE_BINARY_DIR}/Data")
#add_dependencies(VolumeBlogAnimation copy_data)
