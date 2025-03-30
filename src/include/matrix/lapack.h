#ifndef JAGS_LAPACK_H_
#define JAGS_LAPACK_H_

extern "C" {

    void jags_dgesv (const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, int *info);
    
    double jags_dlange (const char *norm, const int *m, const int *n, const double *a, const int *lda, double *work);
    
    void jags_dposv (const char *uplo, const int *n, const int *nrhs, double *a, const int *lda, double *b, const int *ldb, int *info);
    
    void jags_dpotrf (const char *uplo, const int *n, double *a, const int *lda, int *info);
    
    void jags_dpotri (const char *uplo, const int *n, double *a, const int *lda, int *info);
    
    void jags_dpotrs (const char *uplo, const int *n, const int *nrhs, const double *a, const int *lda, double *b, const int *ldb, int *info);
    
    void jags_dsyev (const char *jobz, const char *uplo, const int *n, double *a, const int *lda, double *w, double *work, const int *lwork, int *info);

    void jags_dsymv (const char *uplo, int n, double alpha, const double *a, int lda, const double *x, int incx, double beta, double *y, int incy);

    void jags_dsysv (const char *uplo, const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, double *work, const int *lwork, int *info);

    void jags_dtrtri (const char *uplo, const char *diag, const int *n, double *a, const int *lda, int *info);

    void jags_dgeqrf (const int *m, const int *n, double *a, const int *lda, double *tau, double *work, const int *lwork, int *info);

}

#endif
