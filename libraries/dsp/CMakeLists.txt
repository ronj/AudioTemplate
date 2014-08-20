include_directories(${KISSFFT_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIR})

set(FFTEngine "OOURA" CACHE STRING
    "Choose FFT engine to use. Please keep in mind licenses that apply to different engines.")
set(FFTEngineValues "FFTW3;AppleAccelerate;OOURA")
set_property(CACHE FFTEngine PROPERTY STRINGS ${FFTEngineValues})

set(DSP_SOURCES analysis/fft.cpp
                analysis/kiss_fft.cpp
                analysis/beat_detector.cpp
                effects/AudioFFT.cpp
                effects/FFTConvolver.cpp
                effects/Utilities.cpp
                filters/Bessel.cpp
                filters/Biquad.cpp
                filters/Butterworth.cpp
                filters/Cascade.cpp
                filters/ChebyshevI.cpp
                filters/ChebyshevII.cpp
                filters/Custom.cpp
                filters/Design.cpp
                filters/Elliptic.cpp
                filters/Filter.cpp
                filters/Legendre.cpp
                filters/Param.cpp
                filters/PoleFilter.cpp
                filters/RBJ.cpp
                filters/RootFinder.cpp
                filters/State.cpp)

invent_add_library(dsp ${DSP_SOURCES})
target_link_libraries(dsp ${KISSFFT_LIBRARY})

set(DSP_LIBRARY dsp CACHE INTERNAL "DSP library" FORCE)

if (BUILD_TESTS)
  _subdirs(tests)
endif()
