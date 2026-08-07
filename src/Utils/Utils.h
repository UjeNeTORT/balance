#ifndef UTILS_H
#define UTILS_H

#include <string>

#define PICK_MACRO(_1, NAME, ...) NAME
#define unreachable(...) (PICK_MACRO(__VA_ARGS__, unreachable_1, unreachable_0)(__VA_ARGS__))
#define unreachable_0() (Balance::unreachable_internal((__FILE__), (__LINE__)), (""))
#define unreachable_1(Msg) (Balance::unreachable_internal((__FILE__), (__LINE__), (Msg)))

namespace Balance {

void unreachable_internal(const std::string &File, int Line, const std::string &Msg = "");

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


template <typename T, typename U, typename = void>
struct has_eq : std::false_type {};

template <typename T, typename U>
struct has_eq<T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>())>> : std::true_type {};

template <typename T, typename U>
inline constexpr bool has_eq_v = has_eq<T, U>::value;

} // namespace Balance

#endif // UTILS_H
