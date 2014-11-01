set(OPTIONAL_INCLUDES)
set(OPTIONAL_LIBS)
set(OPTIONAL_SOURCES)

find_package(libsndfile)

if (LIBSNDFILE_FOUND)
  list(APPEND OPTIONAL_INCLUDES ${LIBSNDFILE_INCLUDE_DIR})
  list(APPEND OPTIONAL_LIBS ${LIBSNDFILE_LIBRARY})
  list(APPEND OPTIONAL_SOURCES codecs/sndfile_codec.h codecs/sndfile_codec.cpp)
  add_definitions(-DHAS_SNDFILE)
endif()

find_package(curl)

if (CURL_FOUND)
  list(APPEND OPTIONAL_INCLUDES ${CURL_INCLUDE_DIRS})
  list(APPEND OPTIONAL_LIBS ${CURL_LIBRARIES})
  list(APPEND OPTIONAL_SOURCES data-access/http.h data-access/http.cpp)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${rtaudio_INCLUDE_DIRS} ${MPADEC_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR} ${OPTIONAL_INCLUDES})

set(AUDIO_SOURCES samplerate_control.cpp
                  samplerate_aware.cpp
                  codec_repository.cpp
                  codecs/mpadec_codec.cpp
                  codecs/mpadec_info.cpp
                  data-access/uri.h
                  data-access/uri.cpp
                  data-access/file.h
                  data-access/file.cpp
                  data-access/data_access_factory.cpp
                  data-access/data_access_factory.h
                  ${OPTIONAL_SOURCES})

invent_add_library(audio ${AUDIO_SOURCES})
target_link_libraries(audio ${rtaudio_LIBRARY} ${MPADEC_LIBRARY} ${OPTIONAL_LIBS})

set(AUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Include directories for audio" FORCE)
set(AUDIO_LIBRARY audio CACHE INTERNAL "Audio library" FORCE)
