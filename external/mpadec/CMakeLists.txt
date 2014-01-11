SET(MPADEC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "Include directories for RtAudio")

INCLUDE_DIRECTORIES(${MPADEC_INCLUDE_DIRS} libmpadec)

SET(LIBMPADEC_SOURCES libmpadec/layer1.c
                      libmpadec/layer2.c
                      libmpadec/layer3.c
                      libmpadec/mp3dec.c
                      libmpadec/mpadec.c
                      libmpadec/synth.c
                      libmpadec/tables.c)

ADD_LIBRARY(mpadec STATIC ${LIBMPADEC_SOURCES})
SET(MPADEC_LIBRARIES mpadec CACHE INTERNAL "Libraries for MPADec" FORCE)

#ADD_EXECUTABLE(mpadectest main.c output.c)
#TARGET_LINK_LIBRARIES(mpadectest ${MPADEC_LIBRARIES})
