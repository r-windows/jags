jagsmod_LTLIBRARIES = diag.la

diag_la_CPPFLAGS = -I$(top_srcdir)/src/include

diag_la_LDFLAGS = -module -avoid-version
if WINDOWS
diag_la_LDFLAGS += -no-undefined
endif

diag_la_LIBADD = $(top_builddir)/src/lib/libjags.la

diag_la_SOURCES = diag.cc NodeDensityMonitorFactory.cc DensityEnums.cc	\
LeverageMonitorFactory.cc DensityStat.cc DensityTotalStat.cc		\
LeverageStat.cc LeverageTotalStat.cc LooLeverageStat.cc LooDensityStat.cc

noinst_HEADERS = NodeDensityMonitorFactory.h DensityEnums.h		\
LeverageMonitorFactory.h DensityStat.h DensityTotalStat.h LeverageStat.h \
LeverageTotalStat.h LooLeverageStat.h LooDensityStat.h
