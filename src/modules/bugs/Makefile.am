SUBDIRS = distributions functions samplers

jagsmod_LTLIBRARIES = bugs.la

bugs_la_SOURCES = bugs.cc

bugs_la_CPPFLAGS = -I$(top_srcdir)/src/include

bugs_la_LIBADD = distributions/libbugsdist.la           \
        functions/libbugsfunc.la                        \
        samplers/libbugssampler.la                      \
	$(top_builddir)/src/jrmath/libjrmath.la		\
        $(top_builddir)/src/lib/libjags.la

bugs_la_LDFLAGS = -module -avoid-version

if WINDOWS
bugs_la_LDFLAGS += -no-undefined
endif

### Test library 

if CANCHECK
check_LTLIBRARIES = libbugstest.la
libbugstest_la_SOURCES = testbugs.cc testbugs.h
libbugstest_la_CPPFLAGS = -I$(top_srcdir)/src/include
libbugstest_la_CXXFLAGS = $(CPPUNIT_CFLAGS)
libbugstest_la_LIBADD = functions/libbugsfuntest.la		\
	functions/libbugsfunc.la				\
	distributions/libbugsdisttest.la			\
	distributions/libbugsdist.la				\
	$(top_builddir)/src/modules/base/rngs/libbaserngs.la	\
	$(top_builddir)/src/lib/libtest.la			\
	$(top_builddir)/src/lib/libjags.la 			\
	$(top_builddir)/src/jrmath/libjrmath.la			\
	$(CPPUNIT_LIBS)

if WINDOWS
libbugstest_la_LDFLAGS = -no-undefined
endif

endif
