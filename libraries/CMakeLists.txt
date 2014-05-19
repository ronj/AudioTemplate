SET(LIBRARIES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Internal libraries include path")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

_SUBDIRS(
  audio
  common
  dsp
  nodes
  three
)
