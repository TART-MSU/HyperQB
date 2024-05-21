dnl Check if the compiler supports useful warning options.  There's a few that
dnl we don't use, simply because they're too noisy:
dnl
dnl -ansi (prevents declaration of functions like strdup, and because
dnl        it makes warning in system headers).
dnl -Wconversion (useful in older versions of gcc, but not in gcc 2.7.x)
dnl -Wtraditional (combines too many unrelated messages, only a few useful)
dnl -Wredundant-decls (system headers make this too noisy)
dnl -pedantic
dnl -Wunreachable-code (broken, see GCC PR/7827)
dnl -Wredundant-decls (too many warnings in GLIBC's header with old GCC)
dnl -Wstrict-prototypes (not valid in C++, gcc-3.4 complains)
dnl
dnl A few other options have been left out because they are annoying in C++.


AC_DEFUN([CF_GXX_WARNINGS],
[if test "x$GXX" = xyes; then
  AC_CACHE_CHECK([for g++ warning options], ac_cv_prog_gxx_warn_flags,
  [
  cat > conftest.$ac_ext <<EOF
#line __oline__ "configure"
#include <string>
#include <regex>

// From GCC bug 106159
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106159
struct left {  virtual ~left() {} };
struct right { virtual ~right() {} };
struct both: public left, public right {};

int main(int argc, char *argv[[]])
{
  // This string comparison is here to detect superfluous
  // zero-as-null-pointer-constant errors introduced with GCC 10.1's
  // libstdc++, https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95242
  // This is fixed in g++ 10.2, but has the fix is in the compiler
  // instead of the library, clang++ 10.0 also has trouble with it.
  // If we hit a compiler with this issue, we simply do not use
  // -Wzero-as-null-pointer-constant.
  std::string a{"foo"}, b{"bar"};
  if (b < a)
    return 1;
  // GCC 12 has spurious warnings about ininialized values in regex.
  // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105562
  // We need -Wno-maybe-uninitialized in this case.
  std::regex r{"a"};
  (void)r;
  return argv[[argc-1]] == nullptr;
}
EOF
  cf_save_CXXFLAGS="$CXXFLAGS"
  ac_cv_prog_gxx_warn_flags="-W -Werror"
dnl The following list has options of the form OPT:BAD:GOOD
dnl if -OPT fails we try -OPT -BAD.  If -OPT succeeds we add -GOOD.
  for cf_opt in \
   Wall:Wno-maybe-uninitialized:\
   Wint-to-void-pointer-cast \
   Wzero-as-null-pointer-constant \
   Wcast-align \
   Wpointer-arith \
   Wwrite-strings \
   Wcast-qual::DXTSTRINGDEFINES \
   Wdocumentation \
   Wmissing-declarations \
   Wnoexcept \
   Woverloaded-virtual \
   Wmisleading-indentation \
   Wimplicit-fallthrough \
   Wlogical-op \
   Wduplicated-cond \
   Wnull-dereference \
   Wsuggest-override \
   Wpedantic
  do
     fopt=${cf_opt%%:*}
     CXXFLAGS="$cf_save_CXXFLAGS $ac_cv_prog_gxx_warn_flags -$fopt"
     if AC_TRY_EVAL(ac_compile); then
       ac_cv_prog_gxx_warn_flags="$ac_cv_prog_gxx_warn_flags -$fopt"
       case $cf_opt in
        *:*:);;
        *:*:*)ac_cv_prog_gxx_warn_flags="$ac_cv_prog_gxx_warn_flags -${cf_opt##*:}";;
       esac
     else
       case $cf_opt in
        *::*);;
        *:*:*)
          sopt=${cf_opt%:*}
          sopt=${sopt#*:}
          CXXFLAGS="$cf_save_CXXFLAGS $ac_cv_prog_gxx_warn_flags -$fopt -$sopt"
          if AC_TRY_EVAL(ac_compile); then
             ac_cv_prog_gxx_warn_flags="$ac_cv_prog_gxx_warn_flags -$fopt -$sopt"
          fi;;
        esac
     fi
  done
  rm -f conftest*
  CXXFLAGS="$cf_save_CXXFLAGS"])
fi
AC_SUBST([WARNING_CXXFLAGS], ["${ac_cv_prog_gxx_warn_flags}"])
])
