#pragma once
#include <memory>

/// Stack scoped object (stack based scope lifetime object)
template<typename T, typename... Args>
constexpr auto CreateStackScope(Args&&... args) {
    return T{std::forward<Args>(args)...};
}

/// Heap scoped pointer
template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return Scope<T>(new T(std::forward<Args>(args)...));
}

/// Heap reference counted shared pointer
template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return Ref<T>(new T(std::forward<Args>(args)...));
}
