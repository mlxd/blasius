namespace blasius{

/// @brief 
/// Create device tags with limited hierarchy for CPU and GPUs
namespace devices{

struct CPU {};
struct X64_CPU {};
struct ARM64_CPU {};

struct GPU {};
struct CUDA_GPU : GPU{};
struct ROCM_GPU : GPU{};

}

}