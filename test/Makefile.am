if CANCHECK

# Rules for the test code (use `make check` to execute)
TESTS = base bugs mix glm
check_PROGRAMS = $(TESTS)

## Base module

base_SOURCES = base.cc 
base_CXXFLAGS = $(CPPUNIT_CFLAGS)

base_LDADD = $(top_builddir)/src/modules/base/libbasetest.la $(CPPUNIT_LIBS)

base_CPPFLAGS = -I$(top_srcdir)/src/include	\
	-I$(top_srcdir)/src/modules

## Bugs module

bugs_SOURCES = bugs.cc 
bugs_CXXFLAGS = $(CPPUNIT_CFLAGS)

bugs_LDADD = $(top_builddir)/src/modules/bugs/libbugstest.la $(CPPUNIT_LIBS)

bugs_CPPFLAGS = -I$(top_srcdir)/src/include	\
	-I$(top_srcdir)/src/modules

## Mix module

mix_SOURCES = mix.cc 
mix_CXXFLAGS = $(CPPUNIT_CFLAGS)

mix_LDADD = $(top_builddir)/src/modules/mix/libmixtest.la $(CPPUNIT_LIBS)

mix_CPPFLAGS = -I$(top_srcdir)/src/include	\
	-I$(top_srcdir)/src/modules

## Glm module

glm_SOURCES = glm.cc 
glm_CXXFLAGS = $(CPPUNIT_CFLAGS)

glm_LDADD = $(top_builddir)/src/modules/glm/libglmtest.la $(CPPUNIT_LIBS)

glm_CPPFLAGS = -I$(top_srcdir)/src/include	\
	-I$(top_srcdir)/src/modules

endif
