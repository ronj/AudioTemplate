include_directories(${rtaudio_INCLUDE_DIRS} ${MPADEC_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR})

## Audio examples

file(GLOB audio_examples "audio/*.cpp")

foreach(example ${audio_examples})
  get_filename_component(example_name ${example} NAME_WE)
  add_executable(${example_name} ${example})
  target_link_libraries(${example_name} ${AUDIO_LIBRARY} ${DSP_LIBRARY})
  invent_link_if_needed(${example_name})
  set_target_properties(${example_name} PROPERTIES FOLDER Examples/Audio)
endforeach()


## Three examples

set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH};${EXTERNAL_DIR}")
set(CMAKE_PREFIX_PATH ${EXTERNAL_DIR}/rapidjson ${CMAKE_PREFIX_PATH})

set(SDL2_ROOT "" CACHE PATH
  "The location of the SDL2 install prefix (only used if SDL2 is not yet found)")
if(SDL2_ROOT)
  set(CMAKE_PREFIX_PATH ${SDL2_ROOT} ${CMAKE_PREFIX_PATH})
elseif(MSVC)
  set(CMAKE_PREFIX_PATH ${EXTERNAL_DIR}/sdl2-2.0.3/msvc ${CMAKE_PREFIX_PATH})
elseif(MINGW)
  set(CMAKE_PREFIX_PATH ${EXTERNAL_DIR}/sdl2-2.0.3/mingw ${CMAKE_PREFIX_PATH})
endif()

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(RapidJSON REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${RAPID_JSON_INCLUDE_DIR})

set(THREE_EXAMPLE_DEP_LIBS ${THREE_LIB} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${SDLMAIN_LIBRARY})

## TODO: Remove hard-wired data directory path
set(THREE_RELEASE_BUILD FALSE CACHE BOOL
  "Whether to compile examples for installation (changes data dir from absolute to relative reference")
set(DATA_DIR ${CMAKE_HOME_DIRECTORY}/data)
if (THREE_RELEASE_BUILD)
  add_definitions(-DTHREE_DATA_DIR=".")
else()
  add_definitions(-DTHREE_DATA_DIR="${DATA_DIR}")
endif()

#############

# Copy SDL runtime on Windows
if (MSVC OR MINGW)
  if (THREE_BUILD_EXAMPLES OR THREE_BUILD_TESTS)
    get_filename_component(SDL2_PATH ${SDL2MAIN_LIBRARY} PATH)
    set(SDL2_RUNTIME_LIBRARY "${SDL2_PATH}/SDL2.dll" CACHE INTERNAL "" FORCE)
    message (STATUS "Copying ${SDL2_RUNTIME_LIBRARY} to ${THREE_BINARY_PATH} \n")
    set(THREE_DEPENDS three_depends)
    if (MSVC)
      add_custom_target(
        ${THREE_DEPENDS}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${THREE_BINARY_PATH}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_RUNTIME_LIBRARY} "${THREE_BINARY_PATH}/$<CONFIGURATION>"
        COMMENT "Copying ${SDL2_RUNTIME_LIBRARY}")
    else()
      add_custom_target(
        ${THREE_DEPENDS}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${THREE_BINARY_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_RUNTIME_LIBRARY} "${THREE_BINARY_PATH}"
        COMMENT "Copying ${SDL2_RUNTIME_LIBRARY}")
    endif()
  endif()
endif()

file(GLOB_RECURSE THREE_EXAMPLES_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/extras/*.cpp)
file(GLOB_RECURSE THREE_EXAMPLES_LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/extras/*.h)
set(THREE_EXAMPLES_LIB_SOURCE_FILES ${THREE_EXAMPLES_LIB_SOURCES} ${THREE_EXAMPLES_LIB_HEADERS})
invent_add_library(three_examples ${THREE_EXAMPLES_LIB_SOURCE_FILES})
target_link_libraries(three_examples ${THREE_EXAMPLE_DEP_LIBS})

set(THREE_EXAMPLE_LIBS ${THREE_EXAMPLE_DEP_LIBS} three_examples)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(audio_shader audio_shader.cpp)
target_link_libraries(audio_shader three_examples ${AUDIO_LIBRARY} ${DSP_LIBRARY})

file(GLOB three_examples "three/*.cpp")

foreach(example ${three_examples})
  get_filename_component(example_name ${example} NAME_WE)
  add_executable(${example_name} ${example})
  target_link_libraries(${example_name} three_examples)
  invent_link_if_needed(${example_name})
  set_target_properties(${example_name} PROPERTIES FOLDER Examples/Three)
endforeach()
