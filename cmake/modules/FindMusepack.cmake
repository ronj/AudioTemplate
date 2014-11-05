# - Try to find Musepack
# Once done this will define
#
#  MUSEPACK_FOUND - system has Musepack
#  MUSEPACK_INCLUDE_DIR - the Musepack include directory
#  MUSEPACK_LIBRARIES - Link these to use Musepack
#  MUSEPACK_DEFINITIONS - Compiler switches required for using Musepack
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (MUSEPACK_INCLUDE_DIR AND MUSEPACK_LIBRARIES)
    # in cache already
    set(MUSEPACK_FIND_QUIETLY TRUE)
endif()

include(CheckIncludeFiles)
check_include_files(mpc/mpcdec.h HAVE_MPC_MPCDEC_H)
check_include_files(mpcdec/mpcdec.h HAVE_MPCDEC_MPCDEC_H)
check_include_files(musepack/musepack.h HAVE_MUSEPACK_MUSEPACK_H)

if (HAVE_MPC_MPCDEC_H)
    find_path(MUSEPACK_INCLUDE_DIR mpc/mpcdec.h)
    find_library(MUSEPACK_LIBRARIES NAMES mpcdec)
    set(MPC_HEADER_FILE "<mpc/mpcdec.h>")
elseif (HAVE_MPCDEC_MPCDEC_H)
    find_path(MUSEPACK_INCLUDE_DIR mpcdec/mpcdec.h)
    find_library(MUSEPACK_LIBRARIES NAMES mpcdec)
    set(MPC_HEADER_FILE "<mpcdec/mpcdec.h>")
    set(MPC_OLD_API 1)
elseif (HAVE_MUSEPACK_MUSEPACK_H)
    find_path(MUSEPACK_INCLUDE_DIR musepack/musepack.h)
    find_library(MUSEPACK_LIBRARIES NAMES musepack)
    set(MPC_HEADER_FILE "<musepack/musepack.h>")
    set(MPC_OLD_API 1)
endif()

if (MUSEPACK_INCLUDE_DIR AND MUSEPACK_LIBRARIES)
    set(MUSEPACK_FOUND TRUE)
else ()
    set(MUSEPACK_FOUND FALSE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MUSEPACK DEFAULT_MSG MUSEPACK_INCLUDE_DIR MUSEPACK_LIBRARIES MPC_HEADER_FILE)

# show the MUSE_INCLUDE_DIR and MUSE_LIBRARIES variables only in the advanced view
mark_as_advanced(MUSEPACK_INCLUDE_DIR MUSEPACK_LIBRARIES)