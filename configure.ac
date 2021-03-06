
dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_INIT(include/libstrmanager/Manager.h)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR(m4)


dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

GENERIC_LIBRARY_NAME=libstrmanager

#release versioning
GENERIC_MAJOR_VERSION=0
GENERIC_MINOR_VERSION=1
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=0:1:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed


dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AM_INIT_AUTOMAKE($PACKAGE, 1.0, no-define)

dnl -----------------------------------------------
dnl Checks for programs and libraries
dnl -----------------------------------------------

AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_SANITY_CHECK

AC_LANG_CPLUSPLUS

AC_CHECK_LIB([log4cxx],[main],,AC_MSG_ERROR([You need to have log4cxx Library.]),)

AC_CHECK_LIB([boost_thread], main, [], [
  AC_CHECK_LIB(boost_thread-mt, main, [], [
   AC_CHECK_LIB(boost_thread-gcc, main, [], [
    AC_CHECK_LIB(boost_thread-gcc-mt, main, [], [
      AC_CHECK_LIB(boost_thread-gcc-mt-s, main, [], [
        AC_MSG_ERROR([You need to have Boost C++ Library.])
     ])
   ])
  ])
 ])
])

dnl -----------------------------------------------
dnl Generates Makefile's, configuration files and scripts
dnl -----------------------------------------------

AC_OUTPUT(Makefile \
	  m4/Makefile
          libstrmanager.pc
)
