#					        -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([Automated-Linux-From-Scratch],[0.1],[m4573rh4ck3r@protonmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
# Checks for libraries.
# Checks for header files.
AC_CHECK_HEADERS([stdlib.h stdio.h errno.h string.h sys/types.h unistd.h curl/curl.h])
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
AC_CONFIG_FILES([Makefile
src/Makefile])
LIBCURL_CHECK_CONFIG
AC_OUTPUT
