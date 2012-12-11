# Build Gear autoconfigure script
#
# Process this file with autoconf to produce the configure script.
#

AC_PREREQ([2.68])
AC_INIT([Build Gear], [0.9.10beta], [], [buildgear], [http://www.buildgear.org])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip -Wall -Werror])

# Check for programs
AC_PROG_CXX

# Check for libraries and their interfaces
AC_CHECK_LIB([rt], [clock_gettime], , AC_MSG_ERROR([rt not found]))
AC_CHECK_FUNCS([clock_gettime getcwd strerror])
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

curl_gnutls=1
AC_SEARCH_LIBS([curl_global_init], [curl-gnutls], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_easy_init], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_easy_setopt], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_easy_perform], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_easy_cleanup], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_global_cleanup], , [curl_gnutls=0])
AC_CHECK_FUNC([curl_easy_strerror], , [curl_gnutls=0])
AC_CHECK_HEADERS([curl/curl.h], , [curl_gnutls=0])

if test $curl_gnutls == 0
then
	AC_MSG_ERROR([curl-gnutls not found])
fi

termcap=1
AC_SEARCH_LIBS([tgetent], termcap, , [termcap=0])
AC_CHECK_FUNC([tgetnum], , [termcap=0])
AC_CHECK_FUNC([tgetstr], , [termcap=0])
AC_CHECK_FUNC([putp], , [termcap=0])
AC_CHECK_FUNC([tparm], , [termcap=0])
AC_CHECK_HEADERS([term.h], , [termcap=0])
AC_CHECK_HEADERS([curses.h], , [termcap=0])
if test $termcap == 0
then
   AC_MSG_ERROR([termcap not found])
fi

# Check for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
