namespace blasius {

/// @brief
/// Create device tags with limited hierarchy for CPU and GPUs
namespace devices {

/**
 * @brief Generic CPU type.
 *
 */
struct CPU {};

/**
 * @brief x86_64 / amd64 CPU type.
 *
 */
struct X64_CPU {};

struct ARM64_CPU {};

struct GPU {};
struct CUDA_GPU : GPU {};
struct ROCM_GPU : GPU {};

} // namespace devices

} // namespace blasius