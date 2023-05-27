#=========================================================================
# Toplevel configure.ac for the Modular C++ Build System
#=========================================================================
# Please read the documenation in 'mcppbs-doc.txt' for more details on
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

AC_INIT(proj_name,proj_version,proj_maintainer,proj_abbreviation)
AC_LANG_CPLUSPLUS
AC_CONFIG_SRCDIR([riscv/common.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

m4_include(ax_require_defined.m4)
m4_include(ax_append_flag.m4)
m4_include(ax_check_compile_flag.m4)
m4_include(ax_check_link_flag.m4)
m4_include(ax_append_link_flags.m4)
m4_include(ax_boost_base.m4)
m4_include(ax_boost_asio.m4)
m4_include(ax_boost_regex.m4)

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

#-------------------------------------------------------------------------
# MCPPBS specific program checks
#-------------------------------------------------------------------------
# These macros check to see if we can do a stow-based install and also
# check for an isa simulator suitable for running the unit test programs
# via the makefile.

MCPPBS_PROG_INSTALL

#-------------------------------------------------------------------------
# Checks for header files
#-------------------------------------------------------------------------

AC_HEADER_STDC

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
