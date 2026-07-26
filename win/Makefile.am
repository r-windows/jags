SUBDIRS = runtime64

EXTRA_DIST=jags.nsi AdvUninstLog.nsh README.WIN

clean-local:
	rm -rf inst 

installer:
	makensis -nocd -DVERSION=${PACKAGE_VERSION} \
        -DMAJOR=${JAGS_MAJOR} -DJAGSINC=$(srcdir) \
	-DWINARCH=@host_cpu@ \
	-DLICENSE=$(top_srcdir)/COPYING $(srcdir)/jags.nsi

