set(OPTIONAL_INCLUDES)
set(OPTIONAL_LIBS)

find_package(libsndfile)

if (LIBSNDFILE_FOUND)
  list(APPEND OPTIONAL_INCLUDES ${LIBSNDFILE_INCLUDE_DIR})
  list(APPEND OPTIONAL_LIBS ${LIBSNDFILE_LIBRARY})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${rtaudio_INCLUDE_DIRS} ${MPADEC_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR} ${OPTIONAL_INCLUDES})

set(AUDIO_SOURCES samplerate_control.cpp
                  samplerate_aware.cpp)

invent_add_library(audio ${AUDIO_SOURCES})
target_link_libraries(audio ${rtaudio_LIBRARY} ${MPADEC_LIBRARY} ${OPTIONAL_LIBS})

set(AUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Include directories for audio" FORCE)
set(AUDIO_LIBRARY audio CACHE INTERNAL "Audio library" FORCE)
