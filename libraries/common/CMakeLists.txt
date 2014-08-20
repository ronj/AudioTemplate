include_directories(${LIBRARIES_INCLUDE_DIR})

option(USE_DYNAMIC_LOGLEVELS
       "Enable or disable support for dynamic log levels. A disabled level will not output log statements of that level to the sink." OFF)

if(USE_DYNAMIC_LOGLEVELS)
  add_definitions(-DG2_DYNAMIC_LOGGING)
endif()

set(COMMON_SOURCES logging/filesink.cpp
                   logging/log.cpp
                   logging/loglevels.cpp
                   logging/logmessage.cpp
                   logging/logmessagecapture.cpp
                   logging/logworker.cpp
                   logging/time.cpp)

if (WIN32 OR MINGW)
  set(CRASH_HANDLER_SOURCE logging/crashhandler_win.cpp)
else()
  set(CRASH_HANDLER_SOURCE logging/crashhandler_unix.cpp)
endif()

invent_add_library(common ${COMMON_SOURCES} ${CRASH_HANDLER_SOURCE})

set(COMMON_LIBRARY common CACHE INTERNAL "Common library" FORCE)

if (BUILD_TESTS)
  _SUBDIRS(tests)
endif()
