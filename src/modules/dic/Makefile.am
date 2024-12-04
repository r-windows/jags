jagsmod_LTLIBRARIES = dic.la

dic_la_CPPFLAGS = -I$(top_srcdir)/src/include

dic_la_LDFLAGS = -module -avoid-version
if WINDOWS
dic_la_LDFLAGS += -no-undefined
endif

dic_la_LIBADD = $(top_builddir)/src/lib/libjags.la

dic_la_SOURCES = dic.cc 		\
DevianceMonitorFactory.cc 			\
NodeDensityMonitorFactory.cc    		\
DensityTrace.cc DensityMean.cc DensityVariance.cc			\
DensityTotalTrace.cc  \
DensityEnums.cc PDMean.cc POPTMean.cc PenaltyPV.cc    \
PDTotalTrace.cc \
DensityTotalMean.cc DensityTotalVar.cc \
PenaltyMonitorFactory.cc \
DensityStat.cc DensityTotalStat.cc PDStat.cc PDTotalStat.cc POPTStat.cc \
WAICMonitor.cc WAICMonitorFactory.cc

noinst_HEADERS = 		\
DevianceMonitorFactory.h  	\
NodeDensityMonitorFactory.h  \
DensityTrace.h DensityMean.h DensityVariance.h			\
DensityTotalTrace.h  \
DensityEnums.h PDMean.h POPTMean.h PenaltyPV.h    \
PDTotalTrace.h \
DensityTotalMean.h DensityTotalVar.h \
PenaltyMonitorFactory.h \
DensityStat.h DensityTotalStat.h PDStat.h PDTotalStat.h POPTStat.h \
WAICMonitor.h WAICMonitorFactory.h
