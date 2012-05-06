#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([buildgear], [0.8beta], [bug-report@buildgear.org])
#AC_CONFIG_SRCDIR([src/options.cc])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [clock_gettime])
AC_CHECK_LIB([curl-gnutls], [curl_global_init])
AC_CHECK_LIB([curl-gnutls], [curl_easy_init])
AC_CHECK_LIB([curl-gnutls], [curl_easy_setopt])
AC_CHECK_LIB([curl-gnutls], [curl_easy_perform])
AC_CHECK_LIB([curl-gnutls], [curl_easy_cleanup])
AC_CHECK_LIB([curl-gnutls], [curl_global_cleanup])
AC_CHECK_LIB([curl-gnutls], [curl_easy_strerror])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([clock_gettime getcwd strerror])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
