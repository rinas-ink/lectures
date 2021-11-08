#include <iostream>

int main() {
    {
        const char *s = "hello";
        const char *a = s;  // hello
        const char *b = s + 1;  // ello
        std::cout << (a < b) << "\n";  // Not UB: 'a' and 'b' are in the same array.
    }
    {
        const char *a = "hello";
        const char *b = "ello";
        std::cout << (a < b) << "\n";  // unspecified behavior, may be inconsistent.
    }
    {
        const char *a = "hello";
        const char *b = "hello\0";
        std::cout << (a == b) << "\n";  // Unknown result, not UB.
    }
    {
        const char *a = "hello";
        const char *b = "hello\0";
        bool eq = true;
        for (int i = 0; a[i] || b[i]; i++)
            if (a[i] != b[i]) {
                eq = false;
                break;
            }
        std::cout << eq << "\n";
    }
}
