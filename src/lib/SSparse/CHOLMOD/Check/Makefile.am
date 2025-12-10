noinst_LTLIBRARIES = cholmodcheck.la

cholmodcheck_la_SOURCES = cholmod_check.c  cholmod_read.c  cholmod_write.c

cholmodcheck_la_CPPFLAGS = -DNPARTITION \
-I$(top_srcdir)/src/lib/SSparse/config	\
-I$(top_srcdir)/src/lib/SSparse/CHOLMOD/Include

cholmodcheck_la_LDFLAGS = -no-undefined -module -avoid-version

EXTRA_DIST = License.txt lesser.txt 
