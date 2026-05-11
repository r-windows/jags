### Enable gcd
###
AC_DEFUN([JAGS_GCD], [
have_gcd="no"

AC_ARG_ENABLE([gcd],
    AS_HELP_STRING([--enable-gcd], [Enable Grand Central Dispatch]))

if test "x$enable_gcd" = "xyes" ; then
  AC_CHECK_HEADER(dispatch/dispatch.h)
  if test "${ac_cv_header_dispatch_dispatch_h}" = yes ; then
    AC_CHECK_LIB(dispatch, dispatch_get_global_queue, [have_gcd=yes])
  fi
fi
if test "x${have_gcd}" = "xyes"; then
  AC_DEFINE(HAVE_GCD, 1,
            [Define if you have libdispatch headers and libraries, and want
             to use Grand Central Dispatch for thread management])
  GCD_LIBS="-ldispatch"
  AC_SUBST(GCD_LIBS)
fi
AC_MSG_RESULT([enabling GCD ... ${have_gcd}])
])# JAGS_GCD
