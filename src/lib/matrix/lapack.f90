module jags_lapack
  use, intrinsic :: iso_c_binding, only: c_char, c_int, c_double
  implicit none
  
contains
  
  SUBROUTINE JAGS_DSYEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, INFO )&
       & BIND(C, NAME="jags_dsyev")
    CHARACTER(c_char), INTENT(IN) :: JOBZ, UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, LWORK, N
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * )
    REAL(c_double), INTENT(OUT) :: W( * ), WORK( * )
    EXTERNAL DSYEV
    CALL DSYEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, INFO )
  END SUBROUTINE JAGS_DSYEV
    
  SUBROUTINE JAGS_DGESV( N, NRHS, A, LDA, IPIV, B, LDB, INFO )&
       & BIND(C, NAME="jags_dgesv")
    INTEGER(c_int), INTENT(IN) :: LDA, LDB, N, NRHS
    INTEGER(c_int), INTENT(OUT) :: INFO, IPIV( * )
    REAL(c_double), INTENT(INOUT) :: A( LDA, * ), B( LDB, * )
    CALL DGESV( N, NRHS, A, LDA, IPIV, B, LDB, INFO )
  END SUBROUTINE JAGS_DGESV
  
  SUBROUTINE JAGS_DSYSV( UPLO, N, NRHS, A, LDA, IPIV, B, LDB, WORK,&
       & LWORK, INFO ) BIND(C, NAME="jags_dsysv")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, LDB, LWORK, N, NRHS
    INTEGER(c_int), INTENT(OUT) :: INFO
    INTEGER(c_int), INTENT(OUT) :: IPIV( * )
    REAL(c_double), INTENT(INOUT) :: A( LDA, * ), B( LDB, * ), WORK( * )
    CALL DSYSV( UPLO, N, NRHS, A, LDA, IPIV, B, LDB, WORK, LWORK, INFO )
  END SUBROUTINE JAGS_DSYSV
  
  SUBROUTINE JAGS_DPOSV( UPLO, N, NRHS, A, LDA, B, LDB, INFO )&
       & BIND(C, NAME="jags_dposv")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, LDB, N, NRHS
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * ), B( LDB, * )
    CALL DPOSV( UPLO, N, NRHS, A, LDA, B, LDB, INFO )
  END SUBROUTINE JAGS_DPOSV
  
  SUBROUTINE JAGS_DPOTRF( UPLO, N, A, LDA, INFO )&
       & BIND(C, NAME="jags_dpotrf")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, N
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * )
    CALL DPOTRF( UPLO, N, A, LDA, INFO )
  END SUBROUTINE JAGS_DPOTRF

  SUBROUTINE JAGS_DPOTRI( UPLO, N, A, LDA, INFO )&
       & BIND(C, NAME="jags_dpotri")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, N
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * )
    CALL DPOTRI( UPLO, N, A, LDA, INFO )
  END SUBROUTINE JAGS_DPOTRI
  
  SUBROUTINE JAGS_DPOTRS( UPLO, N, NRHS, A, LDA, B, LDB, INFO )&
       & BIND(C, NAME="jags_dpotrs")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, LDB, N, NRHS
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(IN) :: A( LDA, * )
    REAL(c_double), INTENT(INOUT) :: B( LDB, * )  
    CALL DPOTRS( UPLO, N, NRHS, A, LDA, B, LDB, INFO )
  END SUBROUTINE JAGS_DPOTRS
  
  FUNCTION JAGS_DLANGE( NORM, M, N, A, LDA, WORK )&
       & BIND(C, NAME="jags_dlange")
    REAL(c_double) :: JAGS_DLANGE
    CHARACTER(c_char), INTENT(IN) :: NORM
    INTEGER(c_int), INTENT(IN) :: LDA, M, N
    REAL(c_double), INTENT(IN) :: A( LDA, * )
    REAL(c_double), INTENT(OUT) :: WORK( * )
    REAL(c_double), EXTERNAL :: DLANGE
    JAGS_DLANGE = DLANGE( NORM, M, N, A, LDA, WORK )
  END FUNCTION JAGS_DLANGE
  
  SUBROUTINE JAGS_DTRTRI( UPLO, DIAG, N, A, LDA, INFO )&
       & BIND(C, NAME="jags_dtrtri")
    CHARACTER(c_char), INTENT(IN) :: DIAG, UPLO
    INTEGER(c_int), INTENT(IN) :: LDA, N
    INTEGER(c_int), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * )
    CALL DTRTRI( UPLO, DIAG, N, A, LDA, INFO )
  END SUBROUTINE JAGS_DTRTRI

  SUBROUTINE JAGS_DSYMV( UPLO, N, ALPHA, A, LDA, X, INCX, BETA, Y, INCY)&
       & BIND(C, NAME="jags_dsymv")
    CHARACTER(c_char), INTENT(IN) :: UPLO
    INTEGER(c_int), INTENT(IN) :: N, LDA, INCX, INCY
    REAL(c_double), INTENT(IN) :: ALPHA, BETA
    REAL(c_double), INTENT(IN) :: X(*), A( LDA, *)
    REAL(c_double), INTENT(INOUT) :: Y( * )
    CALL DSYMV( UPLO, N, ALPHA, A, LDA, X, INCX, BETA, Y, INCY)
  END SUBROUTINE JAGS_DSYMV

  SUBROUTINE JAGS_DGEQRF( M, N, A, LDA, TAU, WORK, LWORK, INFO)&
       & BIND(C, NAME="jags_dgeqrf")
    INTEGER(c_int), INTENT(IN) :: M, N, LDA, LWORK
    REAL(c_double), INTENT(OUT) :: INFO
    REAL(c_double), INTENT(INOUT) :: A( LDA, * )
    REAL(c_double), INTENT(OUT) :: TAU( * ), WORK( * )
    CALL DGEQRF(M, N, A, LDA, TAU, WORK, LWORK, INFO)
  END SUBROUTINE JAGS_DGEQRF
  
endmodule jags_lapack    
