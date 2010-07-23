#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(sundriver-libusb, git, foofoA@googlemail.com)
# Checks for cross-compiling
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])
AC_PROG_INSTALL
# Checks for programs.
AC_PROG_CC
# Checks for libraries.
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_CXX
# Checks for libusb

# Checks for libraries.

PKG_CHECK_MODULES(USB, libusb >= 0.1.4,,
                 AC_MSG_ERROR([*** Required libusb >= 0.1.4 not installed ***]))
LIBS="$LIBS $USB_LIBS"
CFLAGS="$CFLAGS $USB_CFLAGS"


#Check for types
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdio.h string.h fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS([gettimeofday memset])
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
