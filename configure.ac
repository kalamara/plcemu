#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(plcemu, 2.2, kalamara@users.sourceforge.net)
AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/plcemu.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
# Checks for libraries.
LT_INIT

# Checks for libraries.

# a function in `-lm':
AC_CHECK_LIB(m, floor)

#  a function in `-lcomedi':
AC_CHECK_LIB(comedi, comedi_close)

#  a function in `-lyaml':
AC_CHECK_LIB(yaml, yaml_parser_initialize)

#  a function in `-lpthread':
AC_CHECK_LIB(pthread, pthread_create)

#  a function in `-lzmq':
AC_CHECK_LIB(zmq, zmq_socket)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h pthread.h yaml.h comedi.h zmq.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strrchr])

AC_ARG_ENABLE([sim],
[  --enable-sim    simulated hardware],
[case "${enableval}" in
  yes) sim=true ;;
  no)  sim=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-sim]) ;;
esac],[sim=true])
AM_CONDITIONAL([SIMULATION], [test x$sim = xtrue])

AC_ARG_ENABLE([comedi],
[  --enable-comedi    comedi hw drivers],
[case "${enableval}" in
  yes) comedi=true ;;
  no)  comedi=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-comedi]) ;;
esac],[comedi=false])
AM_CONDITIONAL([COMEDI], [test x$comedi = xtrue])


AC_ARG_ENABLE([ui],
[  --enable-ui   user interface],
[case "${enableval}" in
  yes) ui=true ;;

  no)  ui=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-ui]) ;;
esac],[ui=false])
AM_CONDITIONAL([UI], [test x$ui = xtrue])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
