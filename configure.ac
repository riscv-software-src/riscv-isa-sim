#=========================================================================
# Toplevel configure.ac for the Modular C++ Build System
#=========================================================================
# Please read the documentation in 'mcppbs-doc.txt' for more details on
# how the Modular C++ Build System works. For most new projects, a
# developer will only need to make the following changes:
#
#  - change the project metadata listed right below
#  - update the list of subprojects via the 'MCPPBS_SUBPROJECTS' macro
#  - possibly add subproject groups if needed to ease configuration
#  - add more configure checks for platform specific configuration
#

#-------------------------------------------------------------------------
# Project metadata
#-------------------------------------------------------------------------

m4_define( proj_name,         [RISC-V ISA Simulator])
m4_define( proj_maintainer,   [Andrew Waterman])
m4_define( proj_abbreviation, [spike])

#-------------------------------------------------------------------------
# Project version information
#-------------------------------------------------------------------------
# Version information is meant to be managed through a version control
# system's tags and revision numbers. In a working copy the version will
# not be defined here (you should just use the version control system's
# mechanisms). When we make a distribution then we can set the version
# here as formed by the scripts/vcs-version.sh script so that the
# distribution knows what version it came from. If you are not using
# version control then it is fine to set this directly.

m4_define( proj_version, [?])

#-------------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------------

AC_INIT([proj_name],[proj_version],[proj_maintainer],[proj_abbreviation])
AC_LANG([C++])
AC_CONFIG_SRCDIR([riscv/common.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

m4_include(m4/ax_require_defined.m4)
m4_include(m4/ax_append_flag.m4)
m4_include(m4/ax_check_compile_flag.m4)
m4_include(m4/ax_check_link_flag.m4)
m4_include(m4/ax_append_link_flags.m4)
m4_include(m4/ax_boost_base.m4)
m4_include(m4/ax_boost_asio.m4)
m4_include(m4/ax_boost_regex.m4)

#-------------------------------------------------------------------------
# Build profiles
#-------------------------------------------------------------------------

AC_ARG_ENABLE([clang-lto],
  [AS_HELP_STRING([--enable-clang-lto@<:@=VERSION@:>@],
    [prefer an optimized host-native Clang ThinLTO build; VERSION selects LLVM tools (default: auto-detect version 21)])],
  [enable_clang_lto=$enableval],
  [enable_clang_lto=auto])

case "$enable_clang_lto" in
  auto)
    spike_llvm_suffix=-21
    spike_clang_lto_required=no
    ;;
  yes)
    spike_llvm_suffix=-21
    spike_clang_lto_required=yes
    ;;
  no)
    spike_clang_lto_required=no
    ;;
  *[[!0-9]]*|'')
    AC_MSG_ERROR([--enable-clang-lto accepts only a numeric LLVM version])
    ;;
  *)
    spike_llvm_suffix="-$enable_clang_lto"
    spike_clang_lto_required=yes
    ;;
esac

spike_use_clang_lto=no

AS_IF([test "x$enable_clang_lto" != xno], [
  spike_clang_cc=clang$spike_llvm_suffix
  spike_clang_cxx=clang++$spike_llvm_suffix
  spike_clang_ar=llvm-ar$spike_llvm_suffix
  spike_clang_ranlib=llvm-ranlib$spike_llvm_suffix
  spike_clang_ld=ld.lld$spike_llvm_suffix

  AS_IF([test "x$enable_clang_lto" = xauto && test "x$build" != "x$host"], [
    AC_MSG_NOTICE([cross-compiling; skipping automatic Clang ThinLTO selection])
  ], [
    AS_IF([test "x$enable_clang_lto" = xauto &&
           { test -n "${CC-}" || test -n "${CXX-}" ||
             test -n "${AR-}" || test -n "${RANLIB-}"; }], [
      AC_MSG_NOTICE([compiler tools were supplied; skipping automatic Clang ThinLTO selection])
    ], [
      CC=${CC:-$spike_clang_cc}
      CXX=${CXX:-$spike_clang_cxx}
      AR=${AR:-$spike_clang_ar}
      RANLIB=${RANLIB:-$spike_clang_ranlib}

      spike_missing_llvm_tools=
      for spike_llvm_tool in "$CC" "$CXX" "$AR" "$RANLIB" \
                             "$spike_clang_ld"; do
        if command -v "$spike_llvm_tool" >/dev/null 2>&1 &&
           "$spike_llvm_tool" --version >/dev/null 2>&1; then
          :
        else
          spike_missing_llvm_tools="$spike_missing_llvm_tools $spike_llvm_tool"
        fi
      done

      AS_IF([test -z "$spike_missing_llvm_tools"], [
        spike_clang_ld_path=`command -v "$spike_clang_ld"`
        spike_clang_ld_flag="--ld-path=$spike_clang_ld_path"

        AC_MSG_CHECKING([whether the Clang ThinLTO toolchain can build a C++ program])
        cat > conftest.c <<'_ACEOF'
int spike_lto_probe(void) { return 0; }
_ACEOF
        cat > conftest.cpp <<'_ACEOF'
#include <thread>
extern "C" int spike_lto_probe(void);
static void spike_thread_probe(void) {}
int main(void) {
  std::thread thread(spike_thread_probe);
  thread.join();
  return spike_lto_probe();
}
_ACEOF
        spike_try_clang_lto ()
        {
          rm -f conftest conftest.o conftest.a
          if test -n "$spike_clang_gcc_flag"; then
            "$CC" "$spike_clang_gcc_flag" -O3 -march=native -flto=thin \
              -c conftest.c -o conftest.o >/dev/null 2>&1 &&
            "$AR" cr conftest.a conftest.o >/dev/null 2>&1 &&
            "$RANLIB" conftest.a >/dev/null 2>&1 &&
            "$CXX" "$spike_clang_gcc_flag" -O3 -march=native -flto=thin \
              "$spike_clang_ld_flag" -pthread conftest.cpp conftest.a \
              -o conftest >/dev/null 2>&1
          else
            "$CC" -O3 -march=native -flto=thin \
              -c conftest.c -o conftest.o >/dev/null 2>&1 &&
            "$AR" cr conftest.a conftest.o >/dev/null 2>&1 &&
            "$RANLIB" conftest.a >/dev/null 2>&1 &&
            "$CXX" -O3 -march=native -flto=thin "$spike_clang_ld_flag" \
              -pthread conftest.cpp conftest.a -o conftest >/dev/null 2>&1
          fi
        }

        spike_clang_gcc_flag=
        spike_clang_probe_ok=no
        AS_IF([spike_try_clang_lto], [
          spike_clang_probe_ok=yes
        ])

        AS_IF([test "x$spike_clang_probe_ok" = xno && command -v g++ >/dev/null 2>&1], [
          spike_system_libstdcxx=`g++ -print-file-name=libstdc++.so 2>/dev/null`
          AS_IF([test -f "$spike_system_libstdcxx"], [
            spike_system_gcc_dir=${spike_system_libstdcxx%/*}
            spike_clang_gcc_flag="--gcc-install-dir=$spike_system_gcc_dir"
            AS_IF([spike_try_clang_lto], [
              spike_clang_probe_ok="yes, using $spike_system_gcc_dir"
            ])
          ])
        ])
        rm -f conftest conftest.c conftest.cpp conftest.o conftest.a
        AC_MSG_RESULT([$spike_clang_probe_ok])

        AS_IF([test "x$spike_clang_probe_ok" != xno], [
          AS_IF([test -n "$spike_clang_gcc_flag"], [
            CC="$CC $spike_clang_gcc_flag"
            CXX="$CXX $spike_clang_gcc_flag"
          ])
          spike_use_clang_lto=yes
        ], [
          AS_IF([test "x$spike_clang_lto_required" = xyes], [
            AC_MSG_ERROR([the requested Clang ThinLTO toolchain cannot build and link a C++ program])
          ], [
            AC_MSG_WARN([Clang ThinLTO toolchain is unusable; falling back to the default toolchain])
            unset CC CXX AR RANLIB
          ])
        ])
      ], [
        AS_IF([test "x$spike_clang_lto_required" = xyes], [
          AC_MSG_ERROR([required Clang ThinLTO tools unavailable:$spike_missing_llvm_tools])
        ], [
          AC_MSG_WARN([Clang ThinLTO tools unavailable:$spike_missing_llvm_tools; falling back to the default toolchain])
          unset CC CXX AR RANLIB
        ])
      ])
    ])
  ])
])

#-------------------------------------------------------------------------
# Checks for programs
#-------------------------------------------------------------------------

AC_PROG_CC
AC_PROG_CXX
AC_CHECK_TOOL([AR],[ar])
AC_CHECK_TOOL([RANLIB],[ranlib])
AC_PATH_PROG([DTC],[dtc],[no])
AS_IF([test x"$DTC" == xno],AC_MSG_ERROR([device-tree-compiler not found]))
AC_DEFINE_UNQUOTED(DTC, ["dtc"], [Executable name of device-tree-compiler])

AC_C_BIGENDIAN

SPIKE_OPT_CFLAGS=
SPIKE_OPT_LDFLAGS=

AS_IF([test "x$spike_use_clang_lto" = xyes], [
  SPIKE_OPT_CFLAGS="-O3 -march=native -flto=thin"
  SPIKE_OPT_LDFLAGS="-flto=thin $spike_clang_ld_flag"
])

AC_SUBST([SPIKE_OPT_CFLAGS])
AC_SUBST([SPIKE_OPT_LDFLAGS])

#-------------------------------------------------------------------------
# MCPPBS specific program checks
#-------------------------------------------------------------------------
# These macros check to see if we can do a stow-based install and also
# check for an isa simulator suitable for running the unit test programs
# via the makefile.

MCPPBS_PROG_INSTALL

#-------------------------------------------------------------------------
# Checks for type
#-------------------------------------------------------------------------

AC_CHECK_TYPE([__int128_t],
              [AC_SUBST([HAVE_INT128],[yes])
              AC_DEFINE([HAVE_INT128], [1], [__int128_t is supported])])

#-------------------------------------------------------------------------
# Default compiler flags
#-------------------------------------------------------------------------

AX_APPEND_LINK_FLAGS([-Wl,--export-dynamic])

AX_CHECK_COMPILE_FLAG([-relocatable-pch], AC_SUBST([HAVE_CLANG_PCH],[yes]))

#-------------------------------------------------------------------------
# Checks for libraries
#-------------------------------------------------------------------------

AC_CHECK_LIB([pthread], [pthread_create], [],
  [AC_MSG_ERROR([libpthread is required])])

AX_BOOST_BASE([1.53])
AX_BOOST_ASIO
AX_BOOST_REGEX

#-------------------------------------------------------------------------
# MCPPBS subproject list
#-------------------------------------------------------------------------
# Order list so that subprojects only depend on those listed earlier.
# The '*' suffix indicates an optional subproject. The '**' suffix
# indicates an optional subproject which is also the name of a group.

MCPPBS_SUBPROJECTS([ fesvr, riscv, disasm, customext, fdt, softfloat, spike_main, spike_dasm ])

#-------------------------------------------------------------------------
# MCPPBS subproject groups
#-------------------------------------------------------------------------
# If a group has the same name as a subproject then you must add the
# '**' suffix in the subproject list above. The list of subprojects in a
# group should be ordered so that subprojets only depend on those listed
# earlier. Here is an example:
#
#  MCPPBS_GROUP( [group-name], [sproja,sprojb,...] )
#

#-------------------------------------------------------------------------
# Output
#-------------------------------------------------------------------------

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([riscv-fesvr.pc])
AC_CONFIG_FILES([riscv-disasm.pc])
AC_CONFIG_FILES([riscv-riscv.pc])
AC_OUTPUT
