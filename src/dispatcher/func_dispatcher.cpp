#include <functional>
#include <mutex>
#include <type_traits>

/**
 * @brief This class defines a CPP interface to call the respective BLAS/LAPACK function dynamically
 * through the provided library.
 *
 */
class FuncDispatcher final {
  public:
    explicit FuncDispatcher() = default;
    ~FuncDispatcher() = default;

    FuncDispatcher(const FuncDispatcher&) = delete;
    FuncDispatcher(FuncDispatcher&&) = delete;
    FuncDispatcher& operator=(const FuncDispatcher&) = delete;
    FuncDispatcher& operator=(FuncDispatcher&&) = delete;

    /**
     * @brief Dynamically dispatch to a BLAS/LAPACK standard function.
     *
     */
    template <class RetT, class Func, class... Args>
        requires std::is_invocable_r_v<RetT, Func, Args...>
    constexpr RetT fn(Func&& func,
                      Args&&... args) noexcept(std::is_nothrow_invocable_r_v<RetT, Func, Args...>) {
        std::lock_guard lk(mtx);

        if constexpr (std::is_void_v<RetT>) {
            std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        } else {
            return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        }
    }

  private:
    mutable std::mutex mtx;
};
