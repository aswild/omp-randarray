AC_INIT(randarray, 1.0)
AM_INIT_AUTOMAKE([-Wall -Werror foreign no-dist])
AM_SILENT_RULES(yes)
AC_PROG_CC

AC_ARG_WITH([openmp], AS_HELP_STRING([--without-openmp], [Disable building with OpenMP]))
AM_CONDITIONAL([WITH_OPENMP], [test "$with_openmp" != "no"])
AM_COND_IF([WITH_OPENMP], [AC_DEFINE([WITH_OPENMP])])

AC_CHECK_HEADERS_ONCE([openssl/md5.h])
AM_CONDITIONAL([WITH_OPENSSL], [test "$ac_cv_header_openssl_md5_h" = "yes"])
AM_COND_IF([WITH_OPENSSL], [AC_DEFINE([WITH_MD5])])

AC_OUTPUT(Makefile)
