INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(KISSFFT_SOURCES kiss_fft.c kiss_fftr.c)

ADD_LIBRARY(kissfft ${LIBRARY_TYPE} ${KISSFFT_SOURCES})

GET_TARGET_PROPERTY(KISSFFT_CFLAGS kissfft COMPILE_FLAGS)

IF(KISSFFT_CFLAGS STREQUAL "KISSFFT_CFLAGS-NOTFOUND")
  SET(KISSFFT_CFLAGS "") # set to empty string
else()
  SET(KISSFFT_CFLAGS "${KISSFFT_CFLAGS} ") # a space to cleanly separate from existing content
endif()

SET(KISSFFT_CFLAGS "${KISSFFT_CFLAGS} -mtune=native -ffast-math -fomit-frame-pointer -unroll-loops")
SET_TARGET_PROPERTIES(kissfft PROPERTIES COMPILE_FLAGS ${KISSFFT_CFLAGS})

SET(KISSFFT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Include directories for KissFFT")
SET(KISSFFT_LIBRARY kissfft CACHE INTERNAL "KissFFT library" FORCE)
