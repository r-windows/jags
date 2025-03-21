jagsmod_LTLIBRARIES = dic.la

dic_la_CPPFLAGS = -I$(top_srcdir)/src/include

dic_la_LDFLAGS = -module -avoid-version
if WINDOWS
dic_la_LDFLAGS += -no-undefined
endif

dic_la_LIBADD = $(top_builddir)/src/lib/libjags.la

dic_la_SOURCES = dic.cc NodeDensityMonitorFactory.cc DensityEnums.cc	\
LeverageMonitorFactory.cc DensityStat.cc DensityTotalStat.cc		\
LeverageStat.cc LeverageTotalStat.cc LooLeverageStat.cc LooDensityStat.cc

noinst_HEADERS = NodeDensityMonitorFactory.h DensityEnums.h		\
LeverageMonitorFactory.h DensityStat.h DensityTotalStat.h LeverageStat.h	\
LeverageTotalStat.h LooLeverageStat.h LooDensityStat.h
