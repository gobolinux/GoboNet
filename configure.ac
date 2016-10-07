#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(GoboNet, 0.3, hisham@gobolinux.org)
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

GOBONET_IFCONFIG="/bin/ifconfig"
GOBONET_WPA_SUPPLICANT="/bin/wpa_supplicant"
GOBONET_DHCPCD="/bin/dhcpcd"
GOBONET_KILLALL="/bin/killall"
GOBONET_IWLIST="/bin/iwlist"

AC_ARG_ENABLE(setuid-install, 
   AS_HELP_STRING([--disable-setuid-install       do not set setuid flags during install]),
   [enable_setuid_install=$enableval], [enable_setuid_install="yes"])
AM_CONDITIONAL(SETUID_INSTALL, test x"$enable_setuid_install" = "xyes")

AC_DEFINE_UNQUOTED(GOBONET_IFCONFIG,       "${GOBONET_IFCONFIG}",       "Path of ifconfig")
AC_DEFINE_UNQUOTED(GOBONET_WPA_SUPPLICANT, "${GOBONET_WPA_SUPPLICANT}", "Path of wpa_supplicant")
AC_DEFINE_UNQUOTED(GOBONET_DHCPCD,         "${GOBONET_DHCPCD}",         "Path of dhcpcd")
AC_DEFINE_UNQUOTED(GOBONET_KILLALL,        "${GOBONET_KILLALL}",        "Path of killall")
AC_DEFINE_UNQUOTED(GOBONET_IWLIST,         "${GOBONET_IWLIST}",         "Path of iwlist")

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
