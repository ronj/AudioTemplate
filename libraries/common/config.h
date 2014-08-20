#ifndef COMMON_CONFIG_HPP
#define COMMON_CONFIG_HPP

#if !defined(CFG_UNUSED)
#define CFG_UNUSED(x) ((void)x)
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#define CFG_HAS_DECLSPEC
#endif

// C++11 support
#if defined(__GNUC__)
#  define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)
#  if GCC_VERSION >= 40300
#    define CFG_HAS_VARIADIC_TEMPLATES 1
#    define CFG_HAS_RVALUE_REFERENCES 1
#  endif
#  if GCC_VERSION >= 40400
#    define CFG_HAS_DELETED_FUNCTIONS 1
#  endif
#  if GCC_VERSION >= 40500
#    define CFG_HAS_EXPLICIT_CONVERSION_OPERATORS 1
#    define CFG_HAS_STRING_LITERALS 1
#  endif
#  if GCC_VERSION >= 40600
#    define CFG_HAS_RANGE_FOR 1
#    define CFG_HAS_CONSTEXPR 1
#  endif
#  if GCC_VERSION >= 40700
#    define CFG_HAS_ALIAS_TEMPLATES 1
#  endif
#elif defined(__clang__)
#  define CFG_HAS_VARIADIC_TEMPLATES __has_feature(cxx_variadic_templates)
#  define CFG_HAS_RVALUE_REFERENCES  __has_feature(cxx_rvalue_references)
#  define CFG_HAS_DELETED_FUNCTIONS  __has_feature(cxx_deleted_functions)
#  define CFG_HAS_EXPLICIT_CONVERSION_OPERATORS __has_feature(cxx_explicit_conversions)
#  define CFG_HAS_RANGE_FOR          __has_feature(cxx_range_for)
#  define CFG_HAS_ALIAS_TEMPLATES    __has_feature(cxx_alias_templates)
#  define CFG_HAS_CONSTEXPR          __has_feature(cxx_constexpr)
#  define CFG_HAS_STRING_LITERALS    __has_feature(cxx_raw_string_literals)
#elif defined(_MSC_VER)
// No variadics, no deleted functions, no fun :(
#  if _MSC_VER > 1500
#    define CFG_HAS_RVALUE_REFERENCES 1
#  endif
#  if _MSC_VER > 1600
#    define CFG_HAS_RANGE_FOR 1
#    if MSC_NOV_2012_CTP || _MSC_VER > 1700
#      define CFG_HAS_VARIADIC_TEMPLATES 1
#      define CFG_HAS_EXPLICIT_CONVERSION_OPERATORS 1
#      define CFG_HAS_DELEGATING_CONSTRUCTORS 1
#    endif
#  endif
#endif

#if !defined(CFG_HAS_VARIADIC_TEMPLATES)
#  define CFG_HAS_VARIADIC_TEMPLATES 0
#endif
#if !defined(CFG_HAS_DELETED_FUNCTIONS)
#  define CFG_HAS_DELETED_FUNCTIONS 0
#endif
#if !defined(CFG_HAS_RVALUE_REFERENCES)
#define CFG_HAS_RVALUE_REFERENCES 0
#endif
#if !defined(CFG_HAS_RANGE_FOR)
# define CFG_HAS_RANGE_FOR 0
#endif
#if !defined(CFG_HAS_EXPLICIT_CONVERSION_OPERATORS)
# define CFG_HAS_EXPLICIT_CONVERSION_OPERATORS 0
#endif
#if !defined(CFG_HAS_ALIAS_TEMPLATES)
# define CFG_HAS_ALIAS_TEMPLATES 0
#endif
#if !defined(CFG_HAS_CONSTEXPR)
# define CFG_HAS_CONSTEXPR 0
#endif
#if !defined(CFG_HAS_STRING_LITERALS)
# define CFG_HAS_STRING_LITERALS 0
#endif

#if CFG_HAS_DELETED_FUNCTIONS
#  define CFG_DECL_DELETE = delete
#  define CFG_DECL_DEFAULT = default
#else
#  define CFG_DECL_DELETE
#  define CFG_DECL_DEFAULT {}
#endif

#if !CFG_HAS_RVALUE_REFERENCES
#  error R-value reference support required
#endif
#if !CFG_HAS_RANGE_FOR
#  error Range-based for loop support required
#endif

#if CFG_HAS_EXPLICIT_CONVERSION_OPERATORS
#  define CFG_EXPLICIT explicit
#else
#  define CFG_EXPLICIT
#endif

#if CFG_HAS_CONSTEXPR
#  define CFG_CONSTEXPR constexpr
#else
#  define CFG_CONSTEXPR const
#endif

#endif // COMMON_CONFIG_HPP
