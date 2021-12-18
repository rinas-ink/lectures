# Lecture 05-210930 (часть)
- [Lecture 05-210930 (часть)](#lecture-05-210930-часть)
  - [Запятая (code)](#запятая-code)
  - [Препроцессор](#препроцессор)
    - [Команды](#команды)
  - [Объявление и определение разных штук](#объявление-и-определение-разных-штук)
    - [Как можно называть переменные и функции](#как-можно-называть-переменные-и-функции)
    - [Способы инициализации](#способы-инициализации)
      - [Инициализация глобальных](#инициализация-глобальных)
    - [The most vexing parse (code)](#the-most-vexing-parse-code)
    - [Рекурсивный вызов функциями друг друга (code)](#рекурсивный-вызов-функциями-друг-друга-code)
  - [Рекурсивность в структурах](#рекурсивность-в-структурах)
    - [**incomplete type** (code)](#incomplete-type-code)
    - [Решение проблемы рекурсивности:](#решение-проблемы-рекурсивности)
    - [Рекурсивность в методах класса](#рекурсивность-в-методах-класса)
    - [Friend и Static похожи (code)](#friend-и-static-похожи-code)
## Запятая ([code](../05-210930/00-before/00-comma.cpp))
- Объединяет два expression вычисляет всегда слева направо, возвращает результат правого. `for, while, if` через запятую не объединить это `statement`, а не `expression`.
- Можно перегружать для двух типов`void operator,(){}`. После этого отрубаются свойства о порядке вычисления аргументов.
- `&&` и `||` тоже сначала вычисляют левый, а потом если надо правый. При перегрузке такое поведение отрубается.

## Препроцессор
([code](../05-210930/01-preprocessor/01-literals-int-str-concat.cpp))
- Склеивает подрад идущие строковые литералы.
- Удаляет комментарии.
- Обрабатывает `string s=R"foo(Hello World)foo"`.
- Обработка команд с решеткой.
- `#include` - просто копирует код из файла. Поэтому до С++11 программа без перевода строки в конце - UB.
### Команды
([one](../05-210930/01-preprocessor/02-ifdef-define.cpp),
[two](../05-210930/01-preprocessor/03a-macro-func.cpp),
[three](../05-210930/01-preprocessor/03b-macro-func.cpp),
[four](../05-210930/01-preprocessor/03c-macro-func.cpp),
[five](../05-210930/01-preprocessor/03d-macro-func.cpp),
[six](../05-210930/01-preprocessor/03e-macro-func.cpp),
[seven](../05-210930/01-preprocessor/04-macro-special.cpp),
[eight](../05-210930/01-preprocessor/05-macro-partial.cpp))
- Можно писать в самом файле, а можно флаг `-DSOME_VAR` при компиляции.
- `#error "I won't compile this!"`
- Нельзя переопределять ключевые слова c++! Иначе UB.
- **Прагмы** - команды компилятору поменять свое поведение. ([code](../05-210930/01-preprocessor/06-pragmas.cpp))

## Объявление и определение разных штук
### Как можно называть переменные и функции
([code](../05-210930/02-declare-define/01-identifiers.cpp))
Нельзя ([полный список](https://stackoverflow.com/a/228797/767632), [история](https://codeforces.com/blog/entry/17747):
```c++
struct foo_t {  // '_t', not UB, but reserved by POSIX.
};
int _;  // UB
int _z;  // UB
[[maybe_unused]] int x__b = 30;  // UB. Зарезервировано под компилято и еще что-то
int _Xy;  // UB
int _;  // OK, but no.
// int true = 10;  // No keywords
// int 10fff = 20;  // Not start with a digit
```
### Способы инициализации
[статья](https://habr.com/ru/company/jugru/blog/469465/)  
([code](../05-210930/02-declare-define/02-initialization.cpp))
1. **Default**: Просто объявили поле в структкре и никак не проинициализировали. Примитивные типы не инициализированы. У нетривиальных вызывается конструктор оп умолчанию.
2. **Value**: Забьет тривиальные типы нулями. В остальном как default.`foo f()`.
   - Вектор использует его для своих элементов.
3. **Direct**: С параметрами. Конструктор от передаваемых параметров.`foo f(1)`
4. **Copy**: Со знаком равентва. Там, где возникают неявные преобразования.`foo f=g`. Происходит при инициализации аргументов функции. ([code](../05-210930/02-declare-define/03b-function-args.cpp))
5. **List**: 
   - *Direct* - явное. `foo f{1,2,3}`
   - *Copy* - неявное. `foo f={1,2,3}`
6. ([code](../05-210930/02-declare-define/03a-reference-initialization.cpp))**Инициализация ссылки**: `int &y=x;`. Теперь x и y не отличить.
> Important:
> 1. Direct vs copy: explicit vs implicit. 
> 2. Do not default-initialization: always initialize fundamental types.
```c++
struct Foo { 
    // 1. Default
    int default_initialized_int;   // not initialized
    std::vector<int> default_initialized_vector;  // default-constructed
    
    // 2. Value
    int di_int_1{};  // value-initialized: 0
    int di_int_2;
    Foo() : di_int_2{} {}
    
};

void foo(double) {}

void bar(std::pair<int, int>) {}

int main() {
    // 1. Default
    int default_initialized_int;  // not initialized
    std::vector<int> default_initialized_vector;  // default-constructed: empty
    Foo default_f;  // default-constructed: '*_int' not initialized, '*_vector' default-constructed
    //------------------------------

    // 2. Value
    int value_initializated_int{};  // value initialization: 0
    std::vector<int> value_initializated_vector{};  // default-constructed: empty
    Foo value_f{};   // default-constructed: '*_int' not initialized, '*_vector' is empty

    auto value_int_1 = int();  // value initialization: 0
    auto value_vec_1 = std::vector<int>();
    auto value_foo_1 = Foo();

    auto value_int_2 = int{};  // value initialization: 0
    auto value_vec_2 = std::vector<int>{};
    auto value_foo_2 = Foo{};
    //------------------------------

    // 3. Direct initialization
    std::vector<int> direct_vec_1(10);
    auto direct_vec_2 = static_cast<std::vector<int>>(10); // Вызвали explicit конструктор от int у векора
    std::vector<int> direct_vec_3(10, 123);
    std::vector<int>(10);  // size() == 10
    //------------------------------

    // Copy initialization
    std::vector<int> copy_vec_1 = direct_vec_1;
    bigint x = 55;
    // std::vector<int> copy_vec_2 = 20;  // constructor is explicit
    foo(10);
    // return something;
    int x;
    int *z = &x;
    //------------------------------

    // Direct list initialization
    std::vector<int> direct_list_vec{1, 2, 3};
    std::vector<int>{10};  // size() == 1
    // return std::vector<int>{1, 2, 3};
    //------------------------------

    // Copy list initialization
    std::vector<int> direct_list_vec = {1, 2, 3};
    bar({1, 2});
    // return {1, 2, 3};
    //------------------------------
    }
```
#### Инициализация глобальных
([code](../05-210930/02-declare-define/02b-global.cpp))
zero + default
- *zero-initialization* - занулили все байты;
- *default* - конструктор по умолчанию.
### The most vexing parse ([code](../05-210930/02-declare-define/02c-vexing-parse.cpp))
- Если что-то похоже на функцию компилятор считает это функцией, поэтому так плохо. Код компилируется не так как кажется.
```c++
struct Foo {
    // int x();  // the most vexing parse: function?????
    int y{};  // ok: default initialization, i.e. 0
};

int main() {
    std::vector<int> v2(10);
    std::vector<int> v4;
    std::vector<int> v1{}; 
    std::vector<int> v3(); // Мы сказали что где-то есть функцис с таким названием, а не создали объект. 
}
```
- Вот еще пример. `Foo f(Bar()) ~~~ Foo f(Bar (*arg) ())` - функция, которая принимает указатель на функцию и возвращает ноль аргументов. Чтобы починить нужно добавить скобки.
```c++
struct Bar {
    explicit Bar() {}
};
struct Foo {
    explicit Foo(Bar) {}
};
int main() {
    // Foo f(Bar());  // the most vexing parse: Foo f(Bar (*arg) ())
    Foo f(   (Bar())  );
}
```
### Рекурсивный вызов функциями друг друга ([code](../05-210930/02-declare-define/04-declare-define-recursive.cpp))
- Программа читается сверху вниз. Для этого полезно делить на объвление и определение. 
  - Параметры по умолчанию у функций лучше в объявлении (только в одном месте). `void bar(int = 10);`
  - Компилятор выбирает только между теми перегрузками, которые видит. ([code](../05-210930/02-declare-define/04b-overloads.cpp))

## Рекурсивность в структурах
### **incomplete type** ([code](../05-210930/03-classes/05-incomplete-type.cpp))
знаем, что такой тип есть но не знаем ничего про него(сколько занимает, какие методы и т.п). Поэтому не можем им пользоваться. 
    - можно взять ссылку или указатель на него, но нельзя просто положить в структуру. 
### Решение проблемы рекурсивности:
```c++
struct Bar;  // incomplete type

struct Foo {
    operator Bar();
};

struct Bar {
    Bar() {}
    Bar(Foo /*arg*/) {}
};

Foo::operator Bar() {
    return Bar{};
}
```
### Рекурсивность в методах класса
([code](../05-210930/03-classes/01-inside-classes.cpp))
Ее нет. Можно из нижнего вызывать верхний метод, но не катит для структур (нужно тоже делать как выше).
### Friend и Static похожи ([code](../05-210930/03-classes/02-static-friend.cpp))
- Статическая функция почти как друг - у нее тоже доступ ко всем полям.
