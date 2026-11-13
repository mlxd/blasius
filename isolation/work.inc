/* Compiled twice: IDX = int (LP64) and int64_t (ILP64). Each routine is checked
 * against an independent residual, so a wrong-width bind shows up as WRONG. */
#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)
#define F(n) CAT(n,SUF)
#define NOTE(r) do { if (getenv("PROV_DEBUG")) fprintf(stderr, "  %-6s worst-so-far=%.2e\n", r, worst); } while (0)
typedef void (*CAT(gemm_t,SUF))(int,int,int,IDX,IDX,IDX,double,const double*,IDX,const double*,IDX,double,double*,IDX);
typedef IDX (*CAT(qrf_t,SUF))(int,IDX,IDX,double*,IDX,double*);
typedef IDX (*CAT(gesv_t,SUF))(int,IDX,IDX,double*,IDX,IDX*,double*,IDX);
typedef IDX (*CAT(syev_t,SUF))(int,char,char,IDX,double*,IDX,double*);
typedef IDX (*CAT(gesdd_t,SUF))(int,char,IDX,IDX,double*,IDX,double*,double*,IDX,double*,IDX);
typedef IDX (*CAT(zheev_t,SUF))(int,char,char,IDX,double complex*,IDX,double*);

static double F(run_all)(unsigned seed, int n) {
    IDX N = n; size_t sz = (size_t)n * n; double worst = 0;
    double* A = malloc(sz * 8), *B = malloc(sz * 8), *C = malloc(sz * 8), *D = malloc(sz * 8), *E = malloc(sz * 8), *w = malloc(n * 8);
    IDX* piv = malloc(n * sizeof(IDX));
    double complex* Z = malloc(sz * 16);
    for (size_t i = 0; i < sz; i++) { seed = seed * 1664525u + 1013904223u; A[i] = (seed >> 8) / 16777216.0 - 0.5; }
    /* dgemm against the textbook product */
    CAT(gemm_t,SUF) gemm = (CAT(gemm_t,SUF))sym("cblas_dgemm");
    if (gemm) {
        gemm(102,111,111,N,N,N,1.0,A,N,A,N,0.0,B,N);
        for (int j = 0; j < 40 && j < n; j++) for (int i = 0; i < 40 && i < n; i++) { double s = 0; for (int k = 0; k < n; k++) s += A[i + (size_t)k*n] * A[k + (size_t)j*n]; double e = fabs(s - B[i + (size_t)j*n]); if (e > worst) worst = e; }
        hits++;
    }
    NOTE("dgemm");
    /* QR: |A'A - R'R| */
    CAT(qrf_t,SUF) qrf = (CAT(qrf_t,SUF))sym("LAPACKE_dgeqrf");
    if (qrf) {
        memcpy(C, A, sz * 8);
        IDX info = qrf(102,N,N,C,N,w); if (info) return 1e30;
        for (int j = 0; j < 30 && j < n; j++) for (int i = 0; i < 30 && i < n; i++) { double s = 0, r = 0; for (int k = 0; k < n; k++) s += A[k + (size_t)i*n]*A[k + (size_t)j*n]; for (int k = 0; k <= i && k <= j; k++) r += C[k + (size_t)i*n]*C[k + (size_t)j*n]; double e = fabs(s - r); if (e > worst) worst = e; }
        hits++;
    }
    NOTE("dgeqrf");
    /* solve: |Ax - b| */
    CAT(gesv_t,SUF) gesv = (CAT(gesv_t,SUF))sym("LAPACKE_dgesv");
    if (gesv) {
        memcpy(C, A, sz * 8); for (int i = 0; i < n; i++) D[i] = i + 1.0;
        IDX info = gesv(102,N,1,C,N,piv,D,N); if (info) return 1e30;
        for (int i = 0; i < n; i++) { double s = 0; for (int k = 0; k < n; k++) s += A[i + (size_t)k*n]*D[k]; double e = fabs(s - (i + 1.0)); if (e > worst) worst = e; }
        hits++;
    }
    NOTE("dgesv");
    /* symmetric eigen: |Av - lv| */
    CAT(syev_t,SUF) syev = (CAT(syev_t,SUF))sym("LAPACKE_dsyev");
    if (syev) {
        for (int j = 0; j < n; j++) for (int i = 0; i < n; i++) C[i + (size_t)j*n] = 0.5*(A[i + (size_t)j*n] + A[j + (size_t)i*n]);
        memcpy(D, C, sz * 8);
        IDX info = syev(102,'V','U',N,D,N,w); if (info) return 1e30;
        for (int v = 0; v < 5 && v < n; v++) for (int i = 0; i < n; i++) { double s = 0; for (int k = 0; k < n; k++) s += C[i + (size_t)k*n]*D[k + (size_t)v*n]; double e = fabs(s - w[v]*D[i + (size_t)v*n]); if (e > worst) worst = e; }
        hits++;
    }
    NOTE("dsyev");
    /* SVD: sum of squared singular values = squared Frobenius norm */
    CAT(gesdd_t,SUF) gesdd = (CAT(gesdd_t,SUF))sym("LAPACKE_dgesdd");
    if (gesdd) {
        memcpy(C, A, sz * 8);
        IDX info = gesdd(102,'A',N,N,C,N,w,D,N,E,N); if (info) return 1e30;
        double f = 0, g = 0; for (size_t i = 0; i < sz; i++) f += A[i]*A[i]; for (int i = 0; i < n; i++) g += w[i]*w[i];
        double e = fabs(f - g) / f; if (e > worst) worst = e;
        hits++;
    }
    NOTE("dgesdd");
    /* complex Hermitian: sum of eigenvalues = trace */
    CAT(zheev_t,SUF) zheev = (CAT(zheev_t,SUF))sym("LAPACKE_zheev");
    if (zheev) {
        int m = n < 200 ? n : 200; double tr = 0;
        for (int j = 0; j < m; j++) for (int i = 0; i < m; i++) Z[i + (size_t)j*m] = i == j ? A[i + (size_t)j*n] : (i < j ? A[i + (size_t)j*n] + I*A[j + (size_t)i*n] : A[j + (size_t)i*n] - I*A[i + (size_t)j*n]);
        for (int i = 0; i < m; i++) tr += creal(Z[i + (size_t)i*m]);
        IDX info = zheev(102,'N','U',(IDX)m,Z,(IDX)m,w); if (info) return 1e30;
        double t2 = 0; for (int i = 0; i < m; i++) t2 += w[i];
        double e = fabs(tr - t2); if (e > worst) worst = e;
        hits++;
    }
    NOTE("zheev");
    /* host allocator churn, including libc-allocated memory freed here */
    for (int i = 0; i < 64; i++) { char* s = strdup("churn"); char* p = malloc(32 + i * 512); p[0] = s[0]; free(p); free(s); }
    free(A); free(B); free(C); free(D); free(E); free(w); free(piv); free(Z);
    return worst;
}
#undef F
#undef NOTE
#undef CAT
#undef CAT2
