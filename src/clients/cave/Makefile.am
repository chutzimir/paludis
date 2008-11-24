AM_CXXFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
	      @PALUDIS_CXXFLAGS@ @PALUDIS_CXXFLAGS_NO_WOLD_STYLE_CAST@ @PALUDIS_CXXFLAGS_VISIBILITY@
DEFS= \
	-DBINDIR=\"$(bindir)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DLIBDIR=\"$(libdir)\" \
	-DPYTHONINSTALLDIR=\"$(PYTHON_INSTALL_DIR)\" \
	-DRUBYINSTALLDIR=\"$(RUBY_INSTALL_DIR)\"

SUBDIRS = .

bin_PROGRAMS = cave

noinst_PROGRAMS = man-cave

command_MANS = \
	cave-help.1 \
	cave-print-categories.1 \
	cave-print-commands.1 \
	cave-print-environment-metadata.1 \
	cave-print-id-contents.1 \
	cave-print-id-metadata.1 \
	cave-print-ids.1 \
	cave-print-owners.1 \
	cave-print-packages.1 \
	cave-print-repositories.1 \
	cave-print-sets.1 \
	cave-print-sync-protocols.1 \
	cave-show.1 \
	cave-sync.1

man_MANS = \
	cave.1 \
	$(command_MANS)

cave.1 : man-cave
	./man-cave > $@

$(command_MANS) : man-cave
	if ! ./man-cave `echo $@ | sed -e 's.^cave-..' -e 's,\.1$$,,'` > $@ ; then rm -f $@ ; exit 1 ; fi

cave_SOURCES = \
	cave.cc

cave_LDADD = \
	libcave.a \
	$(top_builddir)/paludis/libpaludis_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/paludis/args/libpaludisargs_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/paludis/util/libpaludisutil_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/src/output/liboutput.a \
	$(DYNAMIC_LD_LIBS)

man_cave_SOURCES = \
	man_cave.cc

man_cave_LDADD = \
	libcave.a \
	$(top_builddir)/paludis/args/libpaludisargs_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/paludis/args/libpaludisman_@PALUDIS_PC_SLOT@.a \
	$(top_builddir)/paludis/util/libpaludisutil_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/paludis/libpaludismanpagethings_@PALUDIS_PC_SLOT@.la

noinst_LIBRARIES = libcave.a

libcave_a_SOURCES = \
	colour_formatter.cc colour_formatter.hh \
	command.cc command.hh \
	command_line.cc command_line.hh \
	command_command_line.cc command_command_line.hh \
	command_factory.cc command_factory.hh \
	cmd_help.cc cmd_help.hh \
	cmd_print_categories.cc cmd_print_categories.hh \
	cmd_print_commands.cc cmd_print_commands.hh \
	cmd_print_environment_metadata.cc cmd_print_environment_metadata.hh \
	cmd_print_id_contents.cc cmd_print_id_contents.hh \
	cmd_print_id_metadata.cc cmd_print_id_metadata.hh \
	cmd_print_ids.cc cmd_print_ids.hh \
	cmd_print_owners.cc cmd_print_owners.hh \
	cmd_print_packages.cc cmd_print_packages.hh \
	cmd_print_repositories.cc cmd_print_repositories.hh \
	cmd_print_sets.cc cmd_print_sets.hh \
	cmd_print_sync_protocols.cc cmd_print_sync_protocols.hh \
	cmd_show.cc cmd_show.hh \
	cmd_sync.cc cmd_sync.hh \
	exceptions.cc exceptions.hh \
	format_general.cc format_general.hh \
	format_plain_contents_entry.cc format_plain_contents_entry.hh \
	format_plain_metadata_key.cc format_plain_metadata_key.hh \
	format_string.cc format_string.hh \
	formats.cc formats.hh \
	select_format_for_spec.cc select_format_for_spec.hh

#	cmd_config.cc \
#	cmd_contents.cc \
#	cmd_executables.cc \
#	cmd_info.cc \
#	cmd_install.cc \
#	cmd_owner.cc \
#	cmd_print_executables.cc \
#	cmd_print_id_actions.cc \
#	cmd_print_id_masks.cc \
#	cmd_print_repository_formats.cc \
#	cmd_print_repository_metadata.cc \
#	cmd_print_set.cc \
#	cmd_print_sync_protocol_metadata.cc \
#	cmd_regenerate_installable_cache.cc \
#	cmd_regenerate_installed_cache.cc \
#	cmd_report.cc \
#	cmd_uninstall.cc \
#	cmd_uninstall_unused.cc

TESTS_ENVIRONMENT = env \
	TEST_SCRIPT_DIR="$(srcdir)/" \
	PALUDIS_NO_GLOBAL_HOOKS="yes" \
	PALUDIS_NO_XTERM_TITLES="yes" \
	PALUDIS_OPTIONS="" \
	PALUDIS_EBUILD_DIR="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_srcdir)/paludis/repositories/e/ebuild/`" \
	PALUDIS_EBUILD_DIR_FALLBACK="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/repositories/e/ebuild/`" \
	PALUDIS_EXTRA_EBUILD_MODULES_DIRS="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/util/`" \
	PALUDIS_EAPIS_DIR="$(top_srcdir)/paludis/repositories/e/eapis/" \
	PALUDIS_SUFFIXES_FILE="$(top_srcdir)/paludis/repositories/e/ebuild_entries_suffixes.conf" \
	PALUDIS_DISTRIBUTIONS_DIR="$(top_srcdir)/paludis/distributions/" \
	PALUDIS_DISTRIBUTION="gentoo" \
	PALUDIS_REPOSITORY_SO_DIR="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/repositories`" \
	PALUDIS_ENVIRONMENT_SO_DIR="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/environments`" \
	PALUDIS_NO_CHOWN="yupyup" \
	PALUDIS_REDUCED_USERNAME="`id -un`" \
	PALUDIS_OUTPUTWRAPPER_DIR="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/util/`" \
	TEST_OUTPUT_WRAPPER="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/util/outputwrapper`" \
	SYSCONFDIR="$(sysconfdir)" \
	bash $(top_srcdir)/test/run_test.sh bash

TESTS =

EXTRA_DIST = \
	$(man_MANS) \
	$(TESTS)

CLEANFILES = *~ gmon.out *.gcov *.gcno  *.gcda *.loT
DISTCLEANFILES = $(man_MANS) $(noinst_DATA)
MAINTAINERCLEANFILES = Makefile.in

built-sources : $(BUILT_SOURCES)
	for s in `echo $(SUBDIRS) | tr -d .` ; do $(MAKE) -C $$s built-sources || exit 1 ; done

DISTCHECK_DEPS = cave.1

distcheck-deps-local : $(DISTCHECK_DEPS)

distcheck-deps : distcheck-deps-subdirs

distcheck-deps-subdirs :
	for s in $(SUBDIRS) . ; do if test x$$s = x. ; then $(MAKE) distcheck-deps-local || exit 1 ; \
	    else $(MAKE) -C $$s distcheck-deps || exit 1 ; fi ; done
