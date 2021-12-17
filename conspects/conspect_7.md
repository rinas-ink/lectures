# Lecture 07-211013. Преобразования,

## Неявные преобразования

### Порядок преобразований ([code](../07-211013/01-implicit-conversion/01-one-user-defined.cpp))

1. Несколько стандартных преобразований - `long long`  в `int`, числа друг между другом.
2. Одно пользовательское преобразование - сами создаем понструктор или оператор преобразования.
3. Еще стандартые преобразования.

```c++
// https://en.cppreference.com/w/cpp/language/implicit_conversion

struct Foo { // Пользовательское преобразование от int
    Foo(int) {}
};

struct Bar { // Пользовательское преобразование от Foo.
    // Неконстантное нормально преобразуется к константному.
    Bar(const Foo&) {}
};

void call_with_bar(const Bar&) {
}

int main() {
    // Standard conversions like 'Bar -> const Bar' are boring and omitted (опускаются).
    call_with_bar(Bar(Foo(10)));  // no implicit conversions
    call_with_bar(Bar(Foo(10LL)));  // standard implicit conversion long long -> int
    call_with_bar(Foo(10));  // user-defined implicit conversion Foo -> Bar
    call_with_bar(Bar(10));  // user-defined implicit conversion int -> Foo
    call_with_bar(Bar(10LL));  // standard implicit conversion long long -> int + user-defined implicit conversion int -> Bar
    // call_with_bar(10);  // needs two user-defined implicit conversions: int -> Foo -> Bar
}

```

### Пользовательские литералы  ([code](../07-211013/01-implicit-conversion/02-string-oops.cpp))

- Строка - пользовательский тип.

```c++
call_with_foo(Foo(std::string("hello")));
call_with_foo(Foo("hello"));  // const char[6] -> std::string is a 'user-defined' conversion
call_with_foo(std::string("hello"));
// call_with_foo("hello");  // needs two user-defined conversions

{ // Расширенные литералы, чтобы красиво катовать.
    using namespace std::literals;
    call_with_foo("hello"s);  // user-defined literal: "hello"s is std::string
    // [[maybe_unused]] int s;  // 's' is not a reserved name.
}

```

### Про bool  ([one](../07-211013/01-implicit-conversion/03-operator-implicit.cpp), [two](../07-211013/01-implicit-conversion/04-operator-bool-usage.cpp), [three](../07-211013/01-implicit-conversion/05-operator-explicit.cpp) )

```c++
struct Foo {
    operator int() {
        return 10;
    }
};

struct Bar {
    explicit operator bool() { 
        return true;
    }
};

int main() {
    [[maybe_unused]] Foo f;
    [[maybe_unused]] Bar b;
    [[maybe_unused]] bool x1 = f;  // implicit: user-defined + int-to-bool
    [[maybe_unused]] bool x2 = b;  // implicit: user-defined

    if (f) {} // f --> int --> bool
    if (b) {}

    for (; f;) {}
    for (; b;) {}

    while (f) {}
    while (b) {}

    [[maybe_unused]] int x3 = f; // Здесь неявно кастуется к числу
    [[maybe_unused]] int x4 = 10 + b;
    // int x5 = 10 + std::cin; // Не работает, хотя у cin есть преобразование к bool (используется в if)
    
}
```

- Чтобы избежать неявного преобразования нужно добавть `explicit` перед оператором `bool` в классе.
- В `if, while, for(;тут;)` преобразование явное, поэтому не надо писать лишнего.

## 02-classes

### Делегирующий конструктор ([code](../07-211013/02-classes/01-delegating-constructor.cpp))
- Вызываются только так `bigint() : bigint(0)`.
- Во время создания объекта происходит инициализация всех полей (именно "рождение", один раз).
- После вызова делегирующего конструктора нельзя инициализировать поля (их уже проинициализировал делегирующий конструктор). Можно только переприсвоить.
```c++
struct bigint {
    std::vector<int> digits;
    int var;

    bigint(int x) : digits{x % 10, x / 10} {}

    bigint() : bigint(0)/*, var(10)*/ {  // since C++11
        var = 10; // Это не инициализация а переприсваивание.
    }

    bigint(bool) {
        bigint(0); // Создание временного объекта, который сразу умирает.
    }
};

int main() {
    [[maybe_unused]] bigint b1;
    [[maybe_unused]] bigint b2(true);
}
```
### Ref-qualifier ([code](../07-211013/02-classes/02-ref-qualifier.cpp))
- `&` - функцию можно вызывать только на не временных объектах.
- `const &` - const ref qualifier то же самое что ничего, т.к константная ссылка привязывается к чему угодно.
- Вместо ref-qualifier можно `const stack foo()`, но мешает другим средствая языка иногда. 
```c++
class stack {
    std::vector<int> data;

public:
    void push(int x) & {  // ref-qualifier
        data.push_back(x);
    }

    [[nodiscard]] int get() const {
        assert(!data.empty());
        return data.back();
    }

    int pop() & {  // ref-qualifier
        assert(!data.empty());
        int result = data.back();
        data.pop_back();
        // this->data == data
        return result;
    }
};

stack foo() {
    stack s;
    s.push(1);
    s.push(2);
    return s;
}

int main() {
    //foo().push(10); //Нельзя т.к функция ref-qualified. Если сделать foo - const, то тоже не работает
    
    // В stl нет ref-qualified, это работает
    std::vector<int>{1, 2, 3}.push_back(40);
    std::vector<int>{1, 2, 3} = std::vector{4, 5, 6};

    bigint a; // Работает
    (a++) = 10;
     // Не компилируется, т.к слева должен быть lvalue.
    // int b; 
    // b++ = 10;
}
```

### Static ([one](../07-211013/02-classes/03-static-function.cpp))
- М.б статические поля и методы.
Можно так, хотя поле и приватное.
```c++
struct Counter {
private:
    static int calls;  // Declaration.
    int calls_on_creation = calls;
};
Counter c_global;
#if 0
int Counter::calls = 10;  // subtle: const initialization
#else
int Counter::calls = []() {  // subtle: dynamic initialization
    std::cout << "Counter::calls initialized\n";
    return 10;
}();
#endif
```
## Storage duration
- Свойство объекта, как он создается и умирает.
### Automatic ([code](../07-211013/03-storage-duration/02-automatic.cpp))
- От скобки до скобки. Лежат на стеке. Переменная умирает если стала невидимой и не может стать видимой обратно.
- Если есть `shadowing` (имена совпадают), имеем доступ к самой вложенной.
### Static ([code](../07-211013/03-storage-duration/03-static.cpp))
- В какой-то момент инициализируется и живет пока программа не завершится. Это:
    - глобальные переменные (если структура, то и ее поля);
    - локальные, написанные со словом static. Живут от момента выполения строчки до конца программы(даже если функция закончилась).
- Зачем нужно? Например чтобы создать огромный сишный массив(на стеке не хватит места). `static int data[1e9]`.


