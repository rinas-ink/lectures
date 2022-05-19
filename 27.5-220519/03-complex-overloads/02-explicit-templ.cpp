#include <iostream>

void foo(const int&) {
    std::cout << "1\n";
}

void foo(char) {
    std::cout << "2\n";
}

template<typename T>
void foo(T) {
    std::cout << "3\n";
}

template<typename T>
void foo(T&) {
    std::cout << "4\n";
}

int main() {
    foo<int>(10);  // Arg=int. Requested template with first argument=int
                   // Overload set: 3 (T=int), 4 (T=int)
                   // Viable: 3 (T=int)
                   // Best viable: 3
    foo<>(10);  // Same: T are deduced.

    [[maybe_unused]] const int x = 10;
    // foo<>(x);  // Arg=const int. Overload set: 3 (T=int), 4 (T=const int)
               // Viable: 3 (T=int), 4 (T=const int)
               // Best viable: ambiguous, similar conversions.
    foo(x);  // Arg=const int. Overload set: 1, 2, 3 (T=int), 4 (T=const int)
             // Viable: 1, 2, 3 (T=int), 4 (T=const int)
             // Best viable: 1 (same level as 3/4, but not a template).

    int y = 10;
//    foo<>(y);  // Overload set: 3 (T=int), 4 (T=int)
               // Viable: 3 (T=int), 4 (T=int)
               // Best viable: 4 (better argument match)
    foo(y);  // Overload set: 1, 2, 3 (T=int), 4 (T=int)
             // Viable: 1, 2, 3 (T=int), 4 (T=int)
             // Best viable: 1 (templates are typically worse).

    double z = 10.3;
    foo(z);  // Overload set: 1, 2, 3 (T=double), 4 (T=double)
             // Viable: 1, 2, 3 (T=double), 4 (T=double)
             // Best viable: 4 (better argument match)
}
