#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string>
#include <iostream>

#define PICK_MACRO(_1, NAME, ...) NAME
#define unreachable(...) (PICK_MACRO(__VA_ARGS__, unreachable_1, unreachable_0)(__VA_ARGS__))
#define unreachable_0() (Balance::unreachable_internal((__FILE__), (__LINE__)), (""))
#define unreachable_1(Msg) (Balance::unreachable_internal((__FILE__), (__LINE__), (Msg)))

namespace Balance {

void unreachable_internal(const std::string &File, int Line, const std::string &Msg = "");

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

} // namespace Balance

#endif // UTILS_H
