
template <bool HasCBLAS = true, bool HasLAPACKE = true, bool hasCLAPACK = false>
class BLProvider {
    using has_cblas = HasCBLAS;
    using has_lapacke = HasLAPACKE;
    using has_clapack = hasCLAPACK;
};