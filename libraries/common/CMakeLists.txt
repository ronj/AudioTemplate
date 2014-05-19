include_directories(${LIBRARIES_INCLUDE_DIR})

option(USE_DYNAMIC_LOGLEVELS
       "Enable or disable support for dynamic log levels. A disabled level will not output log statements of that level to the sink." OFF)

if(USE_DYNAMIC_LOGLEVELS)
  add_definitions(-DG2_DYNAMIC_LOGGING)
endif()

set(COMMON_SOURCES logging/g2filesink.cpp
                   logging/g2log.cpp
                   logging/g2loglevels.cpp
                   logging/g2logmessage.cpp
                   logging/g2logmessagecapture.cpp
                   logging/g2logworker.cpp
                   logging/g2time.cpp)

if (WIN32 OR MINGW)
  set(CRASH_HANDLER_SOURCE logging/crashhandler_win.cpp)
else()
  set(CRASH_HANDLER_SOURCE logging/crashhandler_unix.cpp)
endif()

add_library(common ${LIBRARY_TYPE} ${COMMON_SOURCES} ${CRASH_HANDLER_SOURCE})

set(COMMON_LIBRARY common CACHE INTERNAL "Common library" FORCE)

if (BUILD_TESTS)
  _SUBDIRS(tests)
endif()
