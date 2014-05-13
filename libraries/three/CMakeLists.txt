if ((NOT HEADER_ONLY) OR BUILD_EXAMPLES)
  set(SDL_ROOT "" CACHE PATH
      "The location of the SDL install prefix (only used if the SDL is not yet found)")
  if(SDL_ROOT)
    set(CMAKE_PREFIX_PATH ${SDL_ROOT} ${CMAKE_PREFIX_PATH})
  elseif(MSVC)
    set(CMAKE_PREFIX_PATH ${EXTERNALS_DIR}/sdl-1.2.15/msvc ${CMAKE_PREFIX_PATH})
  elseif(MINGW)
    set(CMAKE_PREFIX_PATH ${EXTERNALS_DIR}/sdl-1.2.15/mingw ${CMAKE_PREFIX_PATH})
  endif()

  find_package(SDL REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(RapidJSON REQUIRED)

  if (NOT WIN32)
    find_package(GLEW REQUIRED)
  else()
    # Locating GLEW on Windows isn't worth it... just build it
    add_subdirectory(${EXTERNAL_DIR}/glew-1.9.0)

    option(THREE_STATIC_GLEW "Use static glew library." OFF)

    if (THREE_STATIC_GLEW)
      set(GLEW_LIBRARY glews CACHE INTERNAL "" FORCE)
    else()
      set(GLEW_LIBRARY glew CACHE INTERNAL "" FORCE)
    endif()
  endif()

  set(THREE_DEP_LIBS ${GLEW_LIBRARY} ${OPENGL_LIBRARIES} ${SDL_LIBRARY} CACHE INTERNAL "" FORCE)
  set(THREE_DEP_INCLUDES ${SDL_INCLUDE_DIR} CACHE INTERNAL "" FORCE)

  include_directories(${GLEW_INCLUDE_DIR})
  include_directories(${SDL_INCLUDE_DIR})
  include_directories(${RAPID_JSON_INCLUDE_DIR})

  # TODO: Remove explicit glew dependency
  add_definitions(-DTHREE_GLEW)

  ## TODO: Remove hard-wired data directory path
  set(THREE_RELEASE_BUILD FALSE CACHE BOOL
    "Whether to compile examples for installation (changes data dir from absolute to relative reference")
  set(DATA_DIR ${CMAKE_HOME_DIRECTORY}/data)
  if (THREE_RELEASE_BUILD)
    add_definitions(-DTHREE_DATA_DIR=".")
  else()
    add_definitions(-DTHREE_DATA_DIR="${DATA_DIR}")
  endif()
endif()

# Copy SDL runtime on Windows
if (MSVC OR MINGW)
  if ((NOT HEADER_ONLY) OR BUILD_EXAMPLES OR BUILD_TESTS)
    get_filename_component(SDL_PATH ${SDLMAIN_LIBRARY} PATH)
    set(SDL_RUNTIME_LIBRARY "${SDL_PATH}/SDL.dll" CACHE INTERNAL "" FORCE)
    message (STATUS "Copying ${SDL_RUNTIME_LIBRARY} to ${THREE_BINARY_PATH} \n")
    set(THREE_DEPENDS three_depends)
    if (MSVC)
      #add_custom_command(
      add_custom_target(
        ${THREE_DEPENDS}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${THREE_BINARY_PATH}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_RUNTIME_LIBRARY} "${THREE_BINARY_PATH}/$<CONFIGURATION>"
        COMMENT "Copying ${SDL_RUNTIME_LIBRARY}")
    else()
      add_custom_target(
        ${THREE_DEPENDS}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${THREE_BINARY_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_RUNTIME_LIBRARY} "${THREE_BINARY_PATH}"
        COMMENT "Copying ${SDL_RUNTIME_LIBRARY}")
    endif()
  endif()
endif()

if (NOT HEADER_ONLY)
  add_definitions(-DTHREE_SEPARATE_COMPILATION)
  if (NOT STATIC_LIBRARIES)
    add_definitions(-DTHREE_DYN_LINK)
  endif()

  if(${CMAKE_GENERATOR} STREQUAL Xcode)
     add_library(xcode_sdlmain STATIC externals/sdl-1.2.15/xcode/SDLMain.m)
     set(SDLMAIN_LIBRARY xcode_sdlmain)
  endif()

  set(THREE_LIB_SOURCE_FILES impl/src.cpp impl/src_extras.cpp)
  set(THREE_LIB three)

  three_add_library(${THREE_LIB} ${THREE_LIB_SOURCE_FILES})
endif()
