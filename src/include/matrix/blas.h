#ifndef BLAS_H_
#define BLAS_H_

extern "C" {
    void jags_daxpy (const int *n, const double *da, const double *dx, const int *incx, double *dy, const int *incy);

    double jags_ddot (const int *n, const double *dx, const int *incx, const double *dy, const int *incy);

    void jags_dgemm (const char *transa, const char *transb, const int *m, const int *n, const int *k, const double *alpha, const double *a, const int *lda, const double *b, const int *ldb, const double *beta, double *c, const int *ldc);

    void jags_dgemv (const char *trans, const int *m, const int *n, const double *alpha, const double *a, const int *lda, const double *x, const int *incx, const double *beta, double *y, const int *incy);

    void jags_dscal (const int *n, const double *da, double *dx, const int *incx);

    void jags_dsymm (const char *side, const char *uplo, const int *m, const int *n, const double *alpha, const double *a, const int *lda, const double *b, const int *ldb, const double *beta, double *c, const int *ldc);

    void jags_dsyr (const char *uplo, const int *n, const double *alpha, const double *x, const int *incx, double *a, const int *lda);

    void jags_dsyrk (const char *uplo, const char *trans, const int *n, const int *k, const double *alpha, const double *a, const int *lda, const double *beta, double *c, const int *ldc);

    void jags_dtrmm (const char *side, const char *uplo, const char *transa, const char *diag, const int *m, const int *n, const double *alpha, const double *a, const int *lda, double *b, const int *ldb);

    void jags_dtrsv (const char *uplo, const char *trans, const char *diag, const int *n, const double *a, const int *lda, double *x, const int *incx);

    void jags_dcopy (const int* n, double const *X, const int* incx, double *Y, const int *incy);
    
    void jags_dscal (const int* n, double const *alpha, double *X, const int *incx);

}

#endif /* BLAS_H_ */
