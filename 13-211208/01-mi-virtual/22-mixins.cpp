#include <iostream>

struct Base {
    virtual void foo() = 0;  // Implementation is also possible
    virtual void bar() = 0;  // Implementation is also possible
};

struct X : virtual /* !!! */ Base {
    void foo() override {
        std::cout << "X::foo()\n";
    }
};

struct Y : virtual /* !!! */ Base {
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {};

int main() {
    Derived a;
    // Non-ambiguous if all 'Base' are virtual, otherwise 'Derived' is abstract.
    a.foo();
    a.bar();
}
