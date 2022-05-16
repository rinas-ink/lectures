#include <iostream>

struct Foo {
    Foo(int) {}
    int func(int) { std::cout << "1\n"; }
};

struct Bar {
    Bar(int);
    int func(char) { std::cout << "2\n"; }
};

template<typename ...Ts>
struct Hybrid : Ts... {  // Hybrid : Foo, Bar
    Hybrid() : Ts(10)... {}
    // Hybrid() : Foo(10), Bar(10) {}
    using Ts::func...;  // Since C++17
};

int main() {
    Hybrid<Foo, Bar> h;
    // ....
    h.func(10);   // (1)
    h.func('x');  // (2)
}
