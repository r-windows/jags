/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998-2025  The R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  https://www.R-project.org/Licenses/
 */


/* Remap for JAGS */
#define norm_rand jags_norm_rand
#define unif_rand jags_unif_rand
#define exp_rand  jags_exp_rand

/* Private header file for use during compilation of Mathlib */
#ifndef MATHLIB_PRIVATE_H
#define MATHLIB_PRIVATE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* Required by C99 but might be slow */
#ifdef HAVE_LONG_DOUBLE
#  define LDOUBLE long double
#else
#  define LDOUBLE double
#endif

/* To ensure atanpi, cospi,  sinpi, tanpi are defined */
# ifndef __STDC_WANT_IEC_60559_FUNCS_EXT__
#  define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
# endif

#include <math.h>
#include <float.h> /* DBL_MIN etc */

#include <JRmath.h>

/* Used internally only */
double  jags_d1mach(int);
double	jags_gamma_cody(double);

 
/* Copied from R_ext/RS.h */

#ifdef HAVE_F77_UNDERSCORE
# define F77_CALL(x)    x ## _
#else
# define F77_CALL(x)    x
#endif
#define F77_NAME(x)    F77_CALL(x)
#define F77_SUB(x)     F77_CALL(x)
#define F77_COM(x)     F77_CALL(x)
#define F77_COMDECL(x) F77_CALL(x)

/* moved from dpq.h */
#ifdef HAVE_NEARBYINT
# define R_forceint(x)   nearbyint(x)
#else
# define R_forceint(x)   round(x)
#endif
//R >= 3.1.0; previously: (fabs((x) - R_forceint(x)) > 1e-7)
//R >= 4.4.0; previously: (fabs((x) - R_forceint(x)) > 1e-7 * fmax2(1., fabs(x)))
# define R_nonint(x) 	  (fabs((x) - R_forceint(x)) > 1e-9 * fmax2(1., fabs(x)))
/*						       .... maybe change even to ~ 1e-11 or 12 */

#include <stdio.h>
#include <stdlib.h> /* for exit */
#define MATHLIB_ERROR(fmt,x)	{ printf(fmt,x); exit(1); }
#define MATHLIB_WARNING(fmt,x)		printf(fmt,x)
#define MATHLIB_WARNING2(fmt,x,x2)	printf(fmt,x,x2)
#define MATHLIB_WARNING3(fmt,x,x2,x3)	printf(fmt,x,x2,x3)
#define MATHLIB_WARNING4(fmt,x,x2,x3,x4) printf(fmt,x,x2,x3,x4)
#define MATHLIB_WARNING5(fmt,x,x2,x3,x4,x5) printf(fmt,x,x2,x3,x4,x5)
#define MATHLIB_WARNING6(fmt,x,x2,x3,x4,x5,x6) printf(fmt,x,x2,x3,x4,x5,x6)

#define ISNAN(x) (isnan(x)!=0)
// Arith.h defines it
#ifndef R_FINITE
#ifdef HAVE_WORKING_ISFINITE
/* isfinite is defined in <math.h> according to C99 */
# define R_FINITE(x)    isfinite(x)
#else
# define R_FINITE(x)    JR_finite(x)
#endif
#endif
int JR_finite(double);

#define ML_POSINF	(1.0 / 0.0)
#define ML_NEGINF	((-1.0) / 0.0)
#define ML_NAN		(0.0 / 0.0)

#define _(String) String

#define ML_VALID(x)	(!ISNAN(x))

#define ME_NONE		0
/*	no error */
#define ME_DOMAIN	1
/*	argument out of domain */
#define ME_RANGE	2
/*	value out of range */
#define ME_NOCONV	4
/*	process did not converge */
#define ME_PRECISION	8
/*	does not have "full" precision */
#define ME_UNDERFLOW	16
/*	and underflow occurred (important for IEEE)*/


#define ML_WARN_return_NAN { ML_WARNING(ME_DOMAIN, ""); return ML_NAN; }

/* For a long time prior to R 2.3.0 ML_WARNING did nothing.
   We don't report ME_DOMAIN errors as the callers collect ML_NANs into
   a single warning.
 */
#define ML_WARNING(x, s) { \
   if(x > ME_DOMAIN) { \
       char *msg = ""; \
       switch(x) { \
       case ME_DOMAIN: \
	   msg = _("argument out of domain in '%s'\n");	\
	   break; \
       case ME_RANGE: \
	   msg = _("value out of range in '%s'\n");	\
	   break; \
       case ME_NOCONV: \
	   msg = _("convergence failed in '%s'\n");	\
	   break; \
       case ME_PRECISION: \
	   msg = _("full precision may not have been achieved in '%s'\n"); \
	   break; \
       case ME_UNDERFLOW: \
	   msg = _("underflow occurred in '%s'\n");	\
	   break; \
       } \
       MATHLIB_WARNING(msg, s); \
   } \
}

/* Wilcoxon Rank Sum Distribution */

#define WILCOX_MAX 50

#ifdef HAVE_VISIBILITY_ATTRIBUTE
# define attribute_hidden __attribute__ ((visibility ("hidden")))
#else
# define attribute_hidden
#endif

/* Formerly private part of Mathlib.h */

/* always remap internal functions */
#define bd0       	jags_bd0
#define ebd0       	jags_ebd0
#define chebyshev_eval	jags_chebyshev_eval
#define chebyshev_init	jags_chebyshev_init
#define gammalims	jags_gammalims
#define lfastchoose	jags_lfastchoose
#define lgammacor	jags_lgammacor
#define stirlerr       	jags_stirlerr
#define pnchisq_raw   	jags_pnchisq_raw
#define pgamma_raw   	jags_pgamma_raw
#define pnbeta_raw   	jags_pnbeta_raw
#define pnbeta2       	jags_pnbeta2
#define bratio       	jags_bratio

	/* Chebyshev Series */

attribute_hidden int chebyshev_init(double*, int, double);
attribute_hidden double chebyshev_eval(double, const double *, const int);

	/* Gamma and Related Functions */

attribute_hidden void gammalims(double*, double*);
attribute_hidden double lgammacor(double); /* log(gamma) correction */
attribute_hidden double stirlerr(double);  /* Stirling expansion "error" */

attribute_hidden double lfastchoose(double, double);

attribute_hidden double bd0(double, double);
attribute_hidden void ebd0(double, double, double*, double*);

attribute_hidden double pnchisq_raw(double, double, double, double, double,
				     int, Rboolean, Rboolean);
attribute_hidden double pgamma_raw(double, double, int, int);
attribute_hidden double pbeta_raw(double, double, double, int, int);
attribute_hidden double qchisq_appr(double, double, double, int, int, double tol);
attribute_hidden LDOUBLE pnbeta_raw(double, double, double, double, double);
attribute_hidden double pnbeta2(double, double, double, double, double, int, int);

int	jags_i1mach(int);

/* From toms708.c */
attribute_hidden void bratio(double a, double b, double x, double y,
	    		     double *w, double *w1, int *ierr, int log_p);


#endif /* MATHLIB_PRIVATE_H */
