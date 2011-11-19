AC_DEFUN(GEOSTREAM_WITH_DOTCONF,
[AC_ARG_WITH(dotconf,
[  --with-dotconf         compile with dotconf configuration file],
[if test x"${withval-no}" != xno; then
  sic_save_LIBS=$LIBS
  AC_CHECK_LIB(dotconf, dotconf_create)
  if test x"${ac_cv_lib_dotconf_dotconf}" = xno; then
    AC_MSG_ERROR(libdotconf not found)
  fi
  LIBS=$sic_save_LIBS
fi])
AM_CONDITIONAL(WITH_DOTCONF, test x"${with_dotconf-no}" != xno)
])
