INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${RTAUDIO_INCLUDE_DIRS} ${MPADEC_INCLUDE_DIRS})

SET(AUDIO_SOURCES SamplerateControl.cpp SamplerateAware.cpp)

ADD_LIBRARY(audio ${LIBRARY_TYPE} ${AUDIO_SOURCES})

TARGET_LINK_LIBRARIES(audio ${RTAUDIO_LIBRARY} ${MPADEC_LIBRARY})

SET(AUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Include directories for audio")
SET(AUDIO_LIBRARY audio CACHE INTERNAL "Audio library" FORCE)