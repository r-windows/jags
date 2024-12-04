jagsmod_LTLIBRARIES = dic.la

dic_la_CPPFLAGS = -I$(top_srcdir)/src/include

dic_la_LDFLAGS = -module -avoid-version
if WINDOWS
dic_la_LDFLAGS += -no-undefined
endif

dic_la_LIBADD = $(top_builddir)/src/lib/libjags.la

dic_la_SOURCES = dic.cc NodeDensityMonitorFactory.cc DensityEnums.cc	\
PenaltyMonitorFactory.cc DensityStat.cc DensityTotalStat.cc PDStat.cc	\
PDTotalStat.cc POPTStat.cc

noinst_HEADERS = NodeDensityMonitorFactory.h DensityEnums.h		\
PenaltyMonitorFactory.h DensityStat.h DensityTotalStat.h PDStat.h	\
PDTotalStat.h POPTStat.h
