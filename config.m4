
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
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists bzip3; then
      LIBBZIP3_CFLAGS=`$PKG_CONFIG bzip3 --cflags`
      LIBBZIP3_LIBDIR=`$PKG_CONFIG bzip3 --libs`
      LIBBZIP3_VERSON=`$PKG_CONFIG bzip3 --modversion`
    else
      AC_MSG_ERROR(system bzip3 not found)
    fi

    PHP_EVAL_LIBLINE($LIBBZIP3_LIBDIR, BZIP3_SHARED_LIBADD)
    PHP_EVAL_INCLINE($LIBBZIP3_CFLAGS)
  else
    dnl build-in library
    LIBBZIP3_VERSON=1.5.1
    AC_MSG_RESULT(use build-in version $LIBBZIP3_VERSON)

    BZIP3_SOURCES="
      lib/src/libbz3.c
    "

    dnl fixed version
    if test -z "$SED"; then
      LIBBZIP3_SED="sed";
    else
      LIBBZIP3_SED="$SED";
    fi
    $LIBBZIP3_SED -i "s/VERSION/\"$LIBBZIP3_VERSON\"/" PHP_EXT_SRCDIR()/lib/src/libbz3.c

    PHP_ADD_INCLUDE(PHP_EXT_SRCDIR()/lib/include)
  fi

  AC_DEFINE_UNQUOTED(LIBBZIP3_VERSION, "$LIBBZIP3_VERSON", [ ])

  dnl bz3_decode_block ABI
  AS_VERSION_COMPARE([1.5.0],[$LIBBZIP3_VERSON],
                     [decode_block_new=yes],
                     [decode_block_new=yes],
                     [decode_block_new=no])
  if test "$decode_block_new" != "no"; then
    AC_DEFINE(HAVE_BZIP3_DECODE_BLOCK_NEW,1,[ ])
  fi

  PHP_NEW_EXTENSION(bzip3, bzip3.c $BZIP3_SOURCES, $ext_shared,, [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_SUBST(BZIP3_SHARED_LIBADD)

  if test "$PHP_LIBBZIP3" = "no"; then
    PHP_ADD_BUILD_DIR($ext_builddir/lib/src)
  fi
fi
