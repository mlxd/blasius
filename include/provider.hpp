
/**
 * @brief Generic class carrying information about the supported features from the provided
 * BLAS/LAPACK library.
 *
 * @tparam HasCBLAS Indicates CBLAS is provided. Defaults to `true`.
 * @tparam HasLAPACKE Indicates LAPACKE is provided. Defaults to `true`.
 * @tparam hasCLAPACK Indicates CLAPACK is provided. Defaults to `false`.
 */
template <bool HasCBLAS = true, bool HasLAPACKE = true, bool hasCLAPACK = false>
class BLProvider {
  public:
    using has_cblas = HasCBLAS;
    using has_lapacke = HasLAPACKE;
    using has_clapack = hasCLAPACK;
};