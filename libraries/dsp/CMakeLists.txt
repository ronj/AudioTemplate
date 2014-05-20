include_directories(${KISSFFT_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR})

set(DSP_SOURCES analysis/fft.cpp
                analysis/kiss_fft.cpp
                analysis/beat_detector.cpp
                effects/AudioFFT.cpp
                effects/FFTConvolver.cpp
                effects/Utilities.cpp)

invent_add_library(dsp ${DSP_SOURCES})
target_link_libraries(dsp ${KISSFFT_LIBRARY})

set(DSP_LIBRARY dsp CACHE INTERNAL "DSP library" FORCE)

if (BUILD_TESTS)
  _subdirs(tests)
endif()
