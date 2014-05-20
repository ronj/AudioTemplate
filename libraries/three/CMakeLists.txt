include_directories(${LIBRARIES_INCLUDE_DIR})

if (NOT STATIC_LIBRARIES)
  add_definitions(-DTHREE_DYN_LINK)
endif()

if (ANDROID)
  add_definitions(-DTHREE_GLES)
endif()

# TODO Refactor, add groupings, etc
file(GLOB_RECURSE THREE_SOURCES *.cpp)
file(GLOB_RECURSE THREE_HEADERS *.h)

set(THREE_LIB_SOURCE_FILES ${THREE_SOURCES} ${THREE_HEADERS})

set(THREE_LIB three CACHE INTERNAL "three library" FORCE)
invent_add_library(${THREE_LIB} ${THREE_LIB_SOURCE_FILES})
