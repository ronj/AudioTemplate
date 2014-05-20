find_package(libsndfile)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${rtaudio_INCLUDE_DIRS} ${MPADEC_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR} ${LIBSNDFILE_INCLUDE_DIR})

set(AUDIO_SOURCES samplerate_control.cpp
                  samplerate_aware.cpp)

invent_add_library(audio ${AUDIO_SOURCES})
target_link_libraries(audio ${rtaudio_LIBRARY} ${MPADEC_LIBRARY} ${LIBSNDFILE_LIBRARY})

set(AUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Include directories for audio" FORCE)
set(AUDIO_LIBRARY audio CACHE INTERNAL "Audio library" FORCE)
