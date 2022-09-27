
dnl Check PHP version:
AC_MSG_CHECKING(PHP version)
if test ! -z "$phpincludedir"; then
    PHP_VERSION_ID=`grep 'PHP_VERSION_ID' $phpincludedir/main/php_version.h | sed -e 's/.* \([[0-9]]*\)/\1/g' 2>/dev/null`
elif test ! -z "$PHP_CONFIG"; then
    PHP_VERSION_ID=`$PHP_CONFIG --vernum 2>/dev/null`
fi

if test x"$PHP_VERSION_ID" = "x"; then
    AC_MSG_WARN([none])
else
    AC_MSG_RESULT($PHP_VERSION_ID)
fi

PHP_ARG_ENABLE([bzip3],
  [whether to enable bzip3 support],
  [AS_HELP_STRING([--enable-bzip3],
    [Enable bzip3 support])])

PHP_ARG_WITH([libbzip3],
  [whether to use bzip3 system library],
  [AS_HELP_STRING([--with-libbzip3],
    [Use bzip3 system library])],
  [no],
  [no])

if test "$PHP_BZIP3" != "no"; then

  AC_MSG_CHECKING(for bzip3)

  if test "$PHP_LIBBZIP3" != "no"; then
    dnl system library
    SEARCH_PATH="/usr/local /usr"
    SEARCH_FOR="/include/libbz3.h"
    if test -r $PHP_LIBBZIP3/$SEARCH_FOR; then
      LIBBZIP3_DIR=$PHP_LIBBZIP3
    else
      for i in $SEARCH_PATH; do
        if test -r $i/$ISEARCH_FOR; then
          LIBBZIP3_DIR=$i
          break
        fi
      done
    fi
    if test -z "$LIBBZIP3_DIR"; then
      AC_MSG_RESULT(not found)
      AC_MSG_ERROR(Please reinstall the bzip3 library distribution)
    fi
    AC_MSG_RESULT(use system library)
    PHP_ADD_INCLUDE($LIBBZIP3_DIR/include)

    LIBNAME=bzip3
    LIBSYMBOL=bz3_new
    PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
    [
      PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBBZIP3_DIR/$PHP_LIBDIR, BZIP3_SHARED_LIBADD)
      AC_DEFINE(LIBBZIP3_VERSION, "system library", [ ])
    ], [
      AC_MSG_ERROR(could not find usable bzip3 library)
    ], [
      -L$LIBBZIP3_DIR/$PHP_LIBDIR
    ])
  else
    dnl build-in library
    LIBBZIP3_VERSON=1.1.3
    AC_MSG_RESULT(use build-in version $LIBBZIP3_VERSON)
    AC_DEFINE_UNQUOTED(LIBBZIP3_VERSION, "$LIBBZIP3_VERSON", [ ])

    BZIP3_SOURCES="
      lib/src/libbz3.c
    "

    PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/lib/include)
  fi

  PHP_NEW_EXTENSION(bzip3, bzip3.c $BZIP3_SOURCES, $ext_shared)
  PHP_SUBST(BZIP3_SHARED_LIBADD)
fi
