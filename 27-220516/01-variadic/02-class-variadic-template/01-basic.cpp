#include <cstddef>
#include <string>
#include <tuple>

// "Variadic template" is a class template which has a "template parameter pack"
// https://en.cppreference.com/w/cpp/language/parameter_pack
template<typename A, int N, typename ...Ts>
struct Foo {
     // Ts... xs;  // Does not work, use std::tuple instead:
     std::tuple<Ts...> xs;
};
Foo<int, 10> f1;                                        // M = 0
Foo<int, 10, int, int> f2;                              // M = 2
Foo<int, 10, int, Foo<int, 10, char>, std::string> f3;  // M = 3

// error: parameter pack 'Ts' must be at the end of the template parameter list
// template<typename... Ts, typename T> struct Bar {};
