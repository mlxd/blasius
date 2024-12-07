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
struct X64_CPU : CPU {};

/**
* @brief aarch64 CPU type.
* 
*/
struct ARM64_CPU : CPU {};

/**
* @brief Generic GPU type
* 
*/
struct GPU {};

/**
* @brief NVIDIA CUDA GPU type.
* 
*/
struct CUDA_GPU : GPU {};

/**
* @brief AMD ROCM GPU type.
* 
*/
struct ROCM_GPU : GPU {};

} // namespace devices

} // namespace blasius