INCLUDE_DIRECTORIES(${KISSFFT_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR})

SET(DSP_SOURCES analysis/fft.cpp
                analysis/kiss_fft.cpp
                analysis/beat_detector.cpp
                effects/AudioFFT.cpp
                effects/FFTConvolver.cpp
                effects/Utilities.cpp)

ADD_LIBRARY(dsp ${LIBRARY_TYPE} ${DSP_SOURCES})

TARGET_LINK_LIBRARIES(dsp ${KISSFFT_LIBRARY})

SET(DSP_LIBRARY dsp CACHE INTERNAL "DSP library" FORCE)

_SUBDIRS(test)
