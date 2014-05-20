#
# Try to find libSNDFile library and include path.
# Once done this will define
#
# LIBSNDFILE_FOUND
# LIBSNFILE_INCLUDE_DIR
# LIBSNFILE_LIBRARY
#

find_path(LIBSNDFILE_INCLUDE_DIR sndfile.h)

set(LIBSNDFILE_NAMES ${LIBSNDFILE_NAMES} sndfile libsndfile)
find_library(LIBSNDFILE_LIBRARY NAMES ${LIBSNDFILE_NAMES} PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBSNDFILE DEFAULT_MSG LIBSNDFILE_INCLUDE_DIR LIBSNDFILE_LIBRARY)

mark_as_advanced(LIBSNDFILE_INCLUDE_DIR LIBSNDFILE_LIBRARY)
