# Build Gear autoconfigure script
#
# Process this file with autoconf to produce the configure script.
#

AC_PREREQ([2.68])
AC_INIT([Build Gear], [0.9.4beta], [], [buildgear], [http://www.buildgear.org])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip -Wall -Werror])

# Check for programs
AC_PROG_CXX

# Check for libraries and their interfaces
AC_CHECK_LIB([rt], [clock_gettime], , AC_MSG_ERROR([rt not found]))

curl_gnutls=1
AC_CHECK_LIB([curl-gnutls], [curl_global_init], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_easy_init], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_easy_setopt], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_easy_perform], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_easy_cleanup], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_global_cleanup], , [curl_gnutls=0])
AC_CHECK_LIB([curl-gnutls], [curl_easy_strerror], , [curl_gnutls=0])

if test $curl_gnutls == 0
then
	AC_MSG_ERROR([curl-gnutls not found])
fi

# Check for header files
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([curl/curl.h])

# Check for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Check for library functions
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([clock_gettime getcwd strerror])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
