AC_DEFUN([adl_CHECK_PYTHON],
 [AM_PATH_PYTHON([3.6])
  case $PYTHON in
    [[\\/$]]* | ?:[[\\/]]* );;
    *) AC_MSG_ERROR([The PYTHON variable must be set to an absolute filename.]);;
  esac
  AC_CACHE_CHECK([for $am_display_PYTHON includes directory],
    [adl_cv_python_inc],
    [adl_cv_python_inc=`$PYTHON -c "import sys, sysconfig;]
[sys.stdout.write(sysconfig.get_path('include'))" 2>/dev/null`])
  AC_CACHE_CHECK([for $am_display_PYTHON extension suffix],
    [adl_cv_python_ext],
    [adl_cv_python_ext=`$PYTHON -c "import importlib.machinery as i; print(i.EXTENSION_SUFFIXES[[0]])"`])
  AC_SUBST([PYEXT], [${adl_cv_python_ext%.*}])
  AC_SUBST([PYTHONINC], [$adl_cv_python_inc])
  adl_save_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$adl_save_CPPFLAGS -I$PYTHONINC"
  AC_CHECK_HEADERS([Python.h],,
                   [AC_MSG_ERROR([Python's development headers are not installed.

The package to install is often called python3-devel, but that name
might be different in your distribution.  Note that if you do not plan
to use Spot's Python bindings, you may also disable their compilation
by running
  ./configure --disable-python
and in this case you do not need python3-devel installed.])])
  CPPFLAGS=$adl_save_CPPFLAGS
])
