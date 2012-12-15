# Build Gear autoconfigure script
#
# Process this file with autoconf to produce the configure script.
#

AC_PREREQ([2.68])
AC_INIT([Build Gear], [0.9.11beta], [], [buildgear], [http://www.buildgear.org])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip -Wall -Werror])

# Check for programs
AC_PROG_CXX
AC_LANG([C++])


# Check for C++11 compliance
AC_DEFUN([AC_COMPILE_STDCXX_11], [
  AC_CACHE_CHECK(if g++ supports C++11 features without additional flags,
  ac_cv_cxx_compile_cxx11_native,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
  template <typename T>
    struct check final
    {
      static constexpr T value{ __cplusplus };
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
    check_type&& cr = static_cast<check_type&&>(c);

    static_assert(check_type::value == 201103L, "C++11 compiler");],,
  ac_cv_cxx_compile_cxx11_native=yes, ac_cv_cxx_compile_cxx11_native=no)
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 features with -std=c++11,
  ac_cv_cxx_compile_cxx11_cxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++11"
  AC_TRY_COMPILE([
  template <typename T>
    struct check final
    {
      static constexpr T value{ __cplusplus };
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
    check_type&& cr = static_cast<check_type&&>(c);

    static_assert(check_type::value == 201103L, "C++11 compiler");],,
  ac_cv_cxx_compile_cxx11_cxx=yes, ac_cv_cxx_compile_cxx11_cxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 features with -std=gnu++11,
  ac_cv_cxx_compile_cxx11_gxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++11"
  AC_TRY_COMPILE([
  template <typename T>
    struct check final
    {
      static constexpr T value{ __cplusplus };
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c{};
    check_type&& cr = static_cast<check_type&&>(c);

    static_assert(check_type::value == 201103L, "C++11 compiler");],,
  ac_cv_cxx_compile_cxx11_gxx=yes, ac_cv_cxx_compile_cxx11_gxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  if test "$ac_cv_cxx_compile_cxx11_native" = yes ||
     test "$ac_cv_cxx_compile_cxx11_cxx" = yes ||
     test "$ac_cv_cxx_compile_cxx11_gxx" = yes; then
    AC_DEFINE(HAVE_STDCXX_11,,[Define if g++ supports C++11 features. ])
  fi
])

cpp11=1
AC_COMPILE_STDCXX_11
m4_include([m4/ax_check_compile_flag.m4])
AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXFLAGS="$CXXFLAGS -std=c++11"], [cpp11=0])
if test $cpp11 == 0
then
	AC_MSG_ERROR([c++11 compiler support not found])
fi

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
