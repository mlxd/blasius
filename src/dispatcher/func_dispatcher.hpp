#include <functional>
#include <iostream>
#include <mutex>
#include <type_traits>

/**
 * @brief This class defines a CPP interface to call arbitrary functions
 * linked against the dependent library.
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
     * @brief Dynamically dispatch to a given function with provided arguments. To ensure data
     * access and thread-safety, a locking mutex is used on a per call basis.
     */
    template <class Func, class... Args>
    constexpr auto fn(Func&& func, Args&&... args) noexcept -> decltype(auto) {
        std::lock_guard lk(mtx);
        if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>) {
            std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        } else {
            return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        }
    }

  private:
    mutable std::mutex mtx;
};