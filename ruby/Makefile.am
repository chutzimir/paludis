include $(top_srcdir)/misc/common-makefile.am

SUBDIRS = . demos

AM_CXXFLAGS = -I$(top_srcdir) -I$(srcdir)/ \
	      -I$(top_builddir)/ \
	      @PALUDIS_CXXFLAGS_WITHOUT_PEDANTIC@ \
	      @PALUDIS_CXXFLAGS_NO_WMISSING_NORETURN@ \
	      @PALUDIS_CXXFLAGS_NO_WREDUNDANT_DECLS@ \
	      @PALUDIS_CXXFLAGS_NO_WOLD_STYLE_CAST@ \
	      @PALUDIS_CXXFLAGS_NO_WSHADOW@ \
	      @PALUDIS_CXXFLAGS_VISIBILITY@

DEFS= \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DLIBDIR=\"$(libdir)\"

CLEANFILES += Paludis.so .libs/Paludis.so

IF_RUBY_TESTS = \
	action_TEST.rb \
	choice_TEST.rb \
	contents_TEST.rb \
	dep_spec_TEST.rb \
	environment_TEST.rb \
	filter_TEST.rb \
	filtered_generator_TEST.rb \
	generator_TEST.rb \
	log_TEST.rb \
	package_id_TEST.rb \
	paludis_ruby_TEST.rb \
	qualified_package_name_TEST.rb \
	repository_TEST.rb \
	selection_TEST.rb \
	version_spec_TEST.rb

IF_RUBY_SOURCES = \
	action.cc \
	choice.cc \
	contents.cc \
	dep_label.cc \
	dep_spec.cc \
	environment.cc \
	filter.cc \
	filtered_generator.cc \
	generator.cc \
	log.cc \
	mask.cc \
	metadata_key.cc \
	name.cc \
	nice_names-nn.hh nice_names-nn.cc \
	package_id.cc \
	paludis_ruby.cc paludis_ruby.hh \
	qualified_package_name.cc \
	repository.cc \
	selection.cc \
	version_spec.cc

BUILT_SOURCES = \
	nice_names-nn.hh nice_names-nn.cc

EXTRA_DIST = $(IF_RUBY_TESTS) $(IF_RUBY_SOURCES) \
	paludis_ruby_so.cc \
	choice_TEST_setup.sh choice_TEST_cleanup.sh \
	dep_spec_TEST_setup.sh dep_spec_TEST_cleanup.sh \
	environment_TEST_setup.sh environment_TEST_cleanup.sh \
	generator_TEST_setup.sh generator_TEST_cleanup.sh \
	nice_names.nn \
	package_id_TEST_setup.sh package_id_TEST_cleanup.sh \
	paludis_ruby_TEST_setup.sh paludis_ruby_TEST_cleanup.sh \
	repository_TEST_setup.sh repository_TEST_cleanup.sh \
	action_TEST_setup.sh action_TEST_cleanup.sh

if ENABLE_RUBY

lib_LTLIBRARIES = libpaludisruby_@PALUDIS_PC_SLOT@.la

libpaludisruby_@PALUDIS_PC_SLOT@_la_CXXFLAGS =  $(RUBY_CFLAGS) $(AM_CXXFLAGS) -I.
libpaludisruby_@PALUDIS_PC_SLOT@_la_LDFLAGS = -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0

paludis_ruby_so.o : paludis_ruby_so.cc
	if $(LIBTOOL) --tag=CXX --mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) \
	    $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(RUBY_CFLAGS) $(AM_CXXFLAGS) -c \
	    -o $@ $(srcdir)/paludis_ruby_so.cc ; then ln -fs .libs/paludis_ruby_so.o $@ ; else rm -f $@ ; exit 1 ; fi

libpaludisruby_@PALUDIS_PC_SLOT@_la_SOURCES = $(IF_RUBY_SOURCES)
TESTS = $(IF_RUBY_TESTS)

libpaludisruby_@PALUDIS_PC_SLOT@_la_LIBADD = \
	$(top_builddir)/paludis/libpaludis_@PALUDIS_PC_SLOT@.la \
	$(top_builddir)/paludis/util/libpaludisutil_@PALUDIS_PC_SLOT@.la \
	$(PTHREAD_LIBS)

check_DATA = .libs/Paludis.so
rubylibdir = $(RUBY_INSTALL_DIR)
rubylib_DATA = Paludis.so

.libs/Paludis.so : libpaludisruby_@PALUDIS_PC_SLOT@.la paludis_ruby_so.o
	mkdir -p .libs
	$(CXX) -fPIC -shared $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	    $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) \
	    $(AM_LDFLAGS) $(LDFLAGS) $(PTHREAD_LIBS) \
	    $(RUBY_LIBS) \
	    -o $@ \
	    .libs/paludis_ruby_so.o \
	    -L$(top_builddir)/ruby/.libs -lpaludisruby_@PALUDIS_PC_SLOT@

Paludis.so : libpaludisruby_@PALUDIS_PC_SLOT@.la paludis_ruby_so.o
	$(CXX) -fPIC -shared $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	    $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) \
	    $(AM_LDFLAGS) $(LDFLAGS) $(PTHREAD_LIBS) \
	    $(RUBY_LIBS) \
	    -o $@ \
	    .libs/paludis_ruby_so.o \
	    -L$(top_builddir)/ruby/.libs -lpaludisruby_@PALUDIS_PC_SLOT@

endif

nice_names-nn.hh : nice_names.nn $(top_srcdir)/misc/make_nn.bash
	if ! $(top_srcdir)/misc/make_nn.bash --header $(srcdir)/nice_names.nn > $@ ; then rm -f $@ ; exit 1 ; fi

nice_names-nn.cc : nice_names.nn $(top_srcdir)/misc/make_nn.bash
	if ! $(top_srcdir)/misc/make_nn.bash --source $(srcdir)/nice_names.nn > $@ ; then rm -f $@ ; exit 1 ; fi

