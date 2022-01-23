#include <exception>
#include <stdexcept>
#include <iostream>

struct my_exception : std::exception {
    const char *what() const noexcept override {
        return "my_exception";
    }
};

struct my_runtime_error : std::runtime_error {
    my_runtime_error() : std::runtime_error("yay! " + std::to_string(123)) {}
};

int main() {
    try {
        throw my_exception();
    } catch (const std::exception &e) {
        std::cout << e.what() << "\n";
    }

    try {
        throw my_runtime_error();
    } catch (std::exception &e) {
        std::cout << e.what() << "\n";
    }

    try {
        throw my_runtime_error();
    } catch (std::runtime_error &e) {
        std::cout << e.what() << "\n";
    }
}
