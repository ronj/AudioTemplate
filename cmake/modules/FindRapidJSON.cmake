#
# Try to find RapidJSON library and include path.
# Once done this will define
#
# RAPID_JSON_FOUND
# RAPID_JSON_INCLUDE_DIR
#

find_path(RAPID_JSON_INCLUDE_DIR rapidjson/rapidjson.h
        ${CMAKE_SOURCE_DIR}/external/rapidjson/include
        $ENV{PROGRAMFILES}/rapidjson/include
        ${COMPILER_PATH}/PlatformSDK/Include
        $ENV{JSON_ROOT_DIR}/include
        ${GLEW_ROOT_DIR}/include
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where rapidjson/rapidjson.h resides")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RAPID_JSON DEFAULT_MSG RAPID_JSON_INCLUDE_DIR)

mark_as_advanced(RAPID_JSON_INCLUDE_DIR)
