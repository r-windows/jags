noinst_LTLIBRARIES = colamd.la

colamd_la_SOURCES = \
Source/colamd.c \
Source/colamd_global.c

colamd_la_CPPFLAGS = -I$(top_srcdir)/src/lib/SSparse/config	\
-I$(top_srcdir)/src/lib/SSparse/COLAMD/Include

colamd_la_LDFLAGS = -no-undefined -module -avoid-version

noinst_HEADERS = Include/colamd.h

DIST_EXTRA = README.txt
