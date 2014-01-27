SET(LIBRARIES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Internal libraries include path")

_SUBDIRS(
  audio
  dsp
  nodes
)
