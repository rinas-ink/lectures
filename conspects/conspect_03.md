# Lecture 03-210916 (чуть-чуть)
- [Lecture 03-210916 (чуть-чуть)](#lecture-03-210916-чуть-чуть)
  - [STL](#stl)
    - [Structured binding (code)](#structured-binding-code)
## STL
### Structured binding ([code](../03-210916/01-extra-stl/02-structured-binding.cpp))
- Массив фиксированного размера разбирается на составляющие. Создает переменную типа как справа, ее инициализирует, а потом присваивается в x и y.
```c++
std::pair<int, float> p = {10, 20.5};

{
    auto [x, y] = p; // Только auto
    std::cout << x << " " << y << "\n";
}
{
    auto &[x, y] = p; // Берутся ссылки на значения p
    x++;
    std::cout << x << " " << y << "\n";
    std::cout << p.first << " " << p.second << "\n";
}
{ // minmax возвращает пару из двух ссылок на значения. Здесь все ок, т.к значения из ссылок копируются.
    std::pair<int, int> p = std::minmax(30, 20);
    std::cout << p.first << " " << p.second << "\n";  
}
{ // x, y будт не интами, а ссылками(пусть и константными), т.к имеют тот же тип, что выражение справа. Но мы не умеем в многоходовое продление жизни (см. лекцию 4)
    auto [x, y] = std::minmax(30, 20);
    std::cout << x << " " << y << "\n";  // UB
}
```