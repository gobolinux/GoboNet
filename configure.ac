#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(GoboNet, 0.5, hisham@gobolinux.org)
AC_CONFIG_SRCDIR([src/gobonet_backend.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

AC_DEFUN([GN_HELPER_BINARY], [
   gn_default_$1=/bin/$1
   AC_ARG_WITH($1,
      [AS_HELP_STRING([--with-$1=PATHNAME], [Full pathname of $1 binary (default=/bin/$1).])],
      if test -n "$withval"; then
         gn_default_$1="$withval"
      fi,[])
   AC_DEFINE_UNQUOTED(GOBONET_[]translit([$1],[a-z],[A-Z]), "${gn_default_$1}", "Path of $1")
])

GN_HELPER_BINARY(ifconfig)
GN_HELPER_BINARY(wpa_supplicant)
GN_HELPER_BINARY(dhcpcd)
GN_HELPER_BINARY(killall)
GN_HELPER_BINARY(iwlist)
GN_HELPER_BINARY(rfkill)

AC_ARG_ENABLE(setuid-install, 
   AS_HELP_STRING([--disable-setuid-install       do not set setuid flags during install]),
   [enable_setuid_install=$enableval], [enable_setuid_install="yes"])
AM_CONDITIONAL(SETUID_INSTALL, test x"$enable_setuid_install" = "xyes")

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
