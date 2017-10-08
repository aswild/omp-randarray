AC_INIT(randarray, 1.0)
AM_INIT_AUTOMAKE([-Wall -Werror foreign no-dist])
AM_SILENT_RULES(yes)
AC_PROG_CC

AC_ARG_WITH([openmp], AS_HELP_STRING([--without-openmp], [Disable building with OpenMP]))
AM_CONDITIONAL([WITH_OPENMP], [test "$with_openmp" != "no"])
AM_COND_IF([WITH_OPENMP], [AC_DEFINE([WITH_OPENMP], [1])], [AC_DEFINE([WITH_OPENMP], [0])])

AC_OUTPUT(Makefile)
