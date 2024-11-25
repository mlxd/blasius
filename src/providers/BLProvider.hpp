#include <concepts>
#include <string>
#include <type_traits>

/**
* @brief BLAS/LAPACK provider base-class.
* 
* @tparam IndexT 
*/
template <std::integral IndexT>
class BLVariant {
  public:
    using index_type = typename IndexT;

    BLVariant(std::string_view lib_name) : lib_name_{lib_name} {}

  private:
    std::string lib_name_;
};