# - Check whether the compiler supports a given flag.
# CHECK_CXX_COMPILER_FLAG(FLAG VARIABLE)
#
#  FLAG - the compiler flag
#  VARIABLE - variable to store the result
#

include(CheckCXXSourceCompiles)

macro(CHECK_CXX_COMPILER_FLAG _FLAG _RESULT)
  set(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
  set(CMAKE_REQUIRED_DEFINITIONS "${_FLAG}")
  check_cxx_source_compiles("int main() { return 0; }" ${_RESULT})
  set(CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
endmacro()
