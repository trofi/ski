# AC_WITH_MOTIF
# see if motif/lesstif is around

AC_DEFUN([AC_WITH_MOTIF],
[
dnl Check for X11 libraries
AC_REQUIRE([AC_PATH_X])dnl
AC_REQUIRE([AC_PATH_XTRA])dnl

AS_IF([test "x$no_x" != xyes], [
  # we have X11, now check for Motif
  save_CPPFLAGS=$CPPFLAGS
  save_CFLAGS=$CFLAGS
  save_LIBS=$LIBS
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  CFLAGS="$CFLAGS $X_CFLAGS"
  LIBS="$LIBS $X_LIBS $X_PRE_LIBS -lXext -lX11 $X_EXTRA_LIBS"

  AC_CHECK_HEADER(Xm/Xm.h, have_Xm_h=yes)
  AS_IF([test "x$have_Xm_h" = xyes], [
    AC_CHECK_LIB(Xt, XtSetSubvalue, MOTIF_LIBS="-lXt $MOTIF_LIBS", have_libXt=no)
    AS_IF([test "x$have_libXt" = xno], [
       # Xt might depend on libpthread (for ProcessLock())
       AC_CHECK_LIB(pthread, pthread_mutex_init, have_libpthread=yes)
       AS_IF([test "x$have_libpthread" = xyes], [
         # try Xt again w/ -lpthread
         AC_CHECK_LIB(Xt, XtRealloc,
         MOTIF_LIBS="-lXt -lpthread $MOTIF_LIBS",,-lpthread)
       ])
    ])
    AC_CHECK_LIB(Xp, XpQueryExtension, MOTIF_LIBS="-lXp $MOTIF_LIBS", ,$MOTIF_LIBS)
    AC_CHECK_LIB(Xm, XmStringFree, have_libXm=yes,, $MOTIF_LIBS)
  ])

  AS_IF([test "x$have_libXm" = xyes], [
    MOTIF_LIBS="-lXm $MOTIF_LIBS"
    AC_DEFINE(HAVE_MOTIF, 1, [define if you have motif])
    X_INTERFACE_LIBS="$X_LIBS $X_PRE_LIBS -lXext -lX11 $X_EXTRA_LIBS"
    X_INTERFACE_LIBS="$MOTIF_LIBS $X_INTERFACE_LIBS"
  ], [
    AC_MSG_ERROR([Motif libraries not found!])
  ])

  CPPFLAGS=$save_CPPFLAGS
  CFLAGS=$save_CFLAGS
  LIBS=$save_LIBS
]) # no_x != yes
])
