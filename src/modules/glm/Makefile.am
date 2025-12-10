SUBDIRS = samplers distributions

jagsmod_LTLIBRARIES = glm.la

glm_la_SOURCES = glm.cc

glm_la_CPPFLAGS = -I$(top_srcdir)/src/include	\
-I$(top_srcdir)/src/lib/SSparse/config \
-I$(top_srcdir)/src/lib/SSparse/CHOLMOD/Include

glm_la_LIBADD = samplers/libglmsampler.la 			\
        distributions/libglmdist.la                     	\
	$(top_builddir)/src/lib/libjags.la			\
	$(top_builddir)/src/jrmath/libjrmath.la			\
	@FLIBS@

glm_la_LDFLAGS = -module -avoid-version
if WINDOWS
glm_la_LDFLAGS += -no-undefined
endif

### Test library 

if CANCHECK
check_LTLIBRARIES = libglmtest.la
libglmtest_la_SOURCES = testglm.cc testglm.h
libglmtest_la_CPPFLAGS = -I$(top_srcdir)/src/include
libglmtest_la_CXXFLAGS = $(CPPUNIT_CFLAGS)
libglmtest_la_LDFLAGS = $(CPPUNIT_LIBS)
libglmtest_la_LIBADD = samplers/libglmsamptest.la \
	distributions/libglmdisttest.la	\
	distributions/libglmdist.la \
	samplers/libglmsampler.la \
	$(top_builddir)/src/lib/libtest.la \
	$(top_builddir)/src/lib/libjags.la \
	$(top_builddir)/src/jrmath/libjrmath.la \
        $(top_builddir)/src/modules/base/rngs/libbaserngs.la

if WINDOWS
libglmtest_la_LDFLAGS += -no-undefined
endif

endif
