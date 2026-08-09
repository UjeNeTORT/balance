#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <variant>
#include <vector>

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


template <typename T, template <typename...> class Template>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template <typename T, template <typename...> class Template>
inline constexpr bool is_specialization_of_v = is_specialization_of<T, Template>::value;


template<typename... Ts>
struct MakeVectorVariant;

template<typename... Ts>
struct MakeVectorVariant<std::variant<Ts...>> {
    using type = std::variant<std::vector<Ts>...>;
};

template<typename Variant, typename... NewTs>
struct VariantAppend;

template<typename... Ts, typename... NewTs>
struct VariantAppend<std::variant<Ts...>, NewTs...> {
    using type = std::variant<Ts..., NewTs...>;
};

} // namespace Balance

#endif // UTILS_H
