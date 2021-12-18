# Lecture 07-211013.
- [Lecture 07-211013.](#lecture-07-211013)
  - [Неявные преобразования](#неявные-преобразования)
    - [Порядок преобразований (code)](#порядок-преобразований-code)
    - [Пользовательские литералы  (code)](#пользовательские-литералы--code)
    - [Про bool](#про-bool)
  - [02-classes](#02-classes)
    - [Делегирующий конструктор (code)](#делегирующий-конструктор-code)
    - [Ref-qualifier (code)](#ref-qualifier-code)
    - [Static](#static)
  - [Storage duration](#storage-duration)
    - [Automatic (code)](#automatic-code)
    - [Static (code)](#static-code)
      - [Static functions](#static-functions)
    - [Dynamic - на куче](#dynamic---на-куче)
      - [(Common)](#common)
      - [(Синтаксис new)](#синтаксис-new)

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

### Про bool  
- [one](../07-211013/01-implicit-conversion/03-operator-implicit.cpp), [two](../07-211013/01-implicit-conversion/04-operator-bool-usage.cpp), [three](../07-211013/01-implicit-conversion/05-operator-explicit.cpp))

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
- После вызова делегирующего конструктора нельзя инициализировать поля (их уже проинициализировал делегирующий
  конструктор). Можно только переприсвоить.

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

### Static

- ([one](../07-211013/02-classes/03-static-function.cpp))  М.б статические поля и методы.
- ([two](../07-211013/02-classes/04-static-data-public.cpp))  
  **Всегда определяются вне структур** (в файлике с остальными определениями). У статической переменной должно быть одно
  место в памяти на всю программу.  
  Структуры обычно объявляются в хэдерах => могут быть включены в несколько единиц трансляции => у статической
  переменной будет несколько определений на всю программу, а должно быть одно место в памяти на всю программу.

```c++
struct Counter {
public:
    static int calls;  // Declaration.

    static int counter() {
        return ++calls;
    }
};

int Counter::calls = 0;  // Should be in exactly one TU (see ODR).
// float Counter::calls = 0;  // Conflict. Ошибка компиляции.

```

- ([three](../07-211013/02-classes/05-static-data-private.cpp))
    1. Бессмысленно делать статическую функцию `const` т.к у нее нет объекта, который она согла бы менять.
    2. Можно определить приватную статическую переменную и вне структуры, хотя обычно мы не имеем доступ к таким.
- ([four](../07-211013/02-classes/06-static-init-access.cpp)
  , [five](../07-211013/02-classes/07-static-init-non-static.cpp))  
  Статические константы тоже нужно определять ровно один раз вне класса.
    - Не пришем `Counter::CALLS_INIT`, т.к после левой части мы уже "внутри" Counter.
    - Если проинициализировать просто (`#if 1`), то не будет состояния "не проинициализирован", т.к компилятор
      догадается и зашьет в экзэшник. А если по-сложному(через лямбду), то calls проинициализируется нулем (т.к
      глобальная переменная).

```c++
# if 1
int Counter::calls = CALLS_INIT;  // we are "inside" Counter after ::
const int Counter::CALLS_INIT = 10;  // subtle: const initialization
#else
// subtle: dynamic initialization
const int Counter::CALLS_INIT = []() {
    std::cout << "Counter::CALLS_INIT initialized\n";
    return 10;
}();
#endif
```

## Storage duration

- Свойство объекта, как он создается и умирает.

### Automatic ([code](../07-211013/03-storage-duration/02-automatic.cpp))

- От скобки до скобки(или если умрут раньше). Лежат на стеке. Переменная умирает если стала невидимой и не может стать
  видимой обратно.
- Если есть `shadowing` (имена совпадают), имеем доступ к самой вложенной.
    - Такой конструктор парсится однозначно `Foo f(int x): x(x){}`.
    - `g++ name.cpp -Wall -Wextra -Werror -Wshadow` - Последний кидает ворнинги, а `-Werror` их в ошибки.
- Вектор на стеке хранит только чуть-чуть ссылок, значения на куче. Сишный массив весь на стеке.

### Static ([code](../07-211013/03-storage-duration/03-static.cpp))

- В какой-то момент инициализируется и живет пока программа не завершится. Это:
    - глобальные переменные (если структура, то и ее поля);
    - локальные, написанные со словом static. Живут от момента выполения строчки до конца программы(даже если функция
      закончилась). Но добраться в них можем только из той функции, откуда взяли.
- Зачем нужно? Например чтобы создать огромный сишный массив(на стеке не хватит места).
    - Если инициализируем статический вектор по-умолчанию (т.е нулями), то быстро, а если не нулями, то компилятор
      вшивает все инициализации в экзэшник. `static int data[1e9]={1,2,3}`

#### Static functions 
- ([Функция-счетчик](../07-211013/03-storage-duration/04-static-func-ok.cpp))
- ([Статический вектор в функции](../07-211013/03-storage-duration/05-static-func-oops.cpp))  
Хотим статический вектор, чтобы сэкономить на перевыделении памяти. Но если не clear/resize, то не получим путые векторы для каждой ф-ции как хотим.
```c++
void foo(bool condition, int n) {
    if (condition) {
        static std::vector<int> v((std::cout << "inited\n", n));
        v.clear(); // Чтобы те, что заполнены стали 0, т.к resize не занулит
        v.resize(n);
        // OR: v.assign(n);
    }
}
```
### Dynamic - на куче
#### ([Common](../07-211013/03-storage-duration/07-dynamic.cpp))
- `new` - возвращает указатель на объект на куче.
```c++
Foo *f = new Foo; 
delete f; // Нужно вызвать ровно один раз, если два раза, то UB
```
- `clang++-12 name.cpp -g -O3 -fsanitize=adress` или `valgrind --leak-check=full ./exeшник` чтобы отлавливать.
- Можем выделять массивы элементов подряд идущих в памяти.
```c++
Foo *f_arr = new Foo[variable];
delete[] f_arr;
// delete f_arr; - UB. Скорее всего вызовется деструктор только у первой структуры.
```
- Если пытаться удалить то, что выделено не через `new` - UB.
```c++
Foo f_automatic;
Foo *f_automatic_ptr= &f_automatic;
delete f_automatic_ptr; //UB
```
#### ([Синтаксис new](../07-211013/03-storage-duration/08-dynamic-syntax.cpp))
- Способа именить размер уже выделенной памяти нет.
```c++
{
        Foo *f = new Foo;  // Foo f; // default initialization
        delete f;
    }
    {
        Foo *f = new Foo();  // Foo f(); - the most vexing parse // default initialization
        delete f;
    }
    {
        Foo *f = new Foo{};  // default initialization
        delete f;
    }
    {
        Foo *f = new Foo(10);  // direct initialization
        delete f;
    }
    {
        Foo *f = new Foo{10};  // direct list initialization. Отличие от предыдущего например для вектора(если и ту малость, что на стеке хочется в кучу).
        delete f;
    }
    {
        Foo *f_arr = new Foo[5];
        delete[] f_arr;
        // delete f_arr;  // UB
    }
    {
        Foo *f_arr = new Foo[5]{1, 2, 3};  // copy initialization + value initialization
        delete[] f_arr;
    }
```

