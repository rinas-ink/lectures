# Lecture 14.

## Типы выражений (value_category)

```
Each expression has:

1. Non-reference(!) type.
2. Value category: 

┌────────────────────────────┐
│         glvalue            │
│      (Generalized Left)    │
│              ┌─────────────┼───────────────────┐
│              │             │      rvalue       │
│              │             │      (Right)      │
│ ┌──────┐     │ ┌─────────┐ │   ┌────────────┐  │
│ │lvalue│     │ │ xvalue  │ │   │  prvalue   │  │
│ │(Left)│     │ │(eXpired)│ │   │(Pure Right)│  │
│ └──────┘     │ └─────────┘ │   └────────────┘  │
└──────────────┴─────────────┴───────────────────┘

glvalue: "has name", can be polymorphic, has address.
rvalue: "can be moved from"

Special case: `void()`
```

[cppreference](https://en.cppreference.com/w/cpp/language/value_category)

У каждого выражения есть тип (не ссылка) и value category. Далее про него. Это локальное свойство выражения.
Проявляется, когда используем (например, возвращаем из функции).  
[examples_of_lval_rval_xval](../14-220110/01-move-semantics/01-value-category.cpp)

1. lval
    - Есть имя.
    - Мб полиморфными.
    - Мб слева от знака равенства.
    - Нельзя мувать.
2. prvalue
    - Нет имени.
    - Мб спава от знака равенства.
    - Встроенные операторы(не перегруженные) обычно возвращают такое значение.
3. xvalue
    - Есть имя, но объект не нужен, скоро умрет.
    - Получается, например, при муве.

> std::move is almost exactly that: `static_cast<std::vector<int>&&>(vec);`

> `&` - возвращается ссылка lvalue, `&&` - rvalue.

[examples_of_rlval_refs](../14-220110/01-move-semantics/02-refs.cpp)

4. lvalue ссылки.
    - Привязываются только к lval.

```c++
 // lvalue references, can only bind to lvalue
 int &x1 = /* should be lvalue */ ((((x))));
 // Нельзя к rval:
 int &x4 = /* prvalue */ (/* lvalue */ x + /* prvalue */ 10 + /* prvalue */ bar());  // CE
 int &x5 = /* xvalue is rvalue, CE */ std::move(x);
```

5. rvalue refs
    - Привязываются к rval = prval + xval.
    - У prval можем продлить жизнь. У xval жизнь не продлевается.
    - Если преобразуем prval к xval (например муваем), то жизнь продлить нельзя.

```c++
 // rvalue references, can only bind to rvalue (xvalue + prvalue)
 int &&y1 = /* prvalue */ 10; // lifetime extension: lifetime of the temporary is extended to lifetime of y2
 int &&y2b = /* xvalue */ std::move(/* prvalue */ bar());  // no lifetime extension, accessing y2b is UB.
     // One should never move(prvalue).
 int &&y3 = /* xvalue */ std::move(x);
 // Нельзя к lval
 int &&y4 = /* lvalue, CE */ x;
```

6. Константные lval
    - Привязываются к всему. Продлевают жижнь prval.
7. Константные rval - бесполезны.

```c++
// const lvalue references, can bind anywhere for historical reasons.
 const int &z1 = /* lvalue */ x;
 const int &z2 = /* prvalue */ 10;  // lifetime extension
 const int &z3 = /* xvalue */ std::move(x);  // move(x) == static_cast<int&&>(x)
 const int &z4 = bar();  // lifetime extension

 // Also possible, but mostly useless.
 const int &&zz1 = 10;
 // const int &&zz2 = x1;

 int (&(zzz)) = x;  // Добавление скобок не меняет lval
 ((x)) = 20;
```

[Rvalue lifetime disaster](https://www.youtube.com/watch?v=zzkpTbJiFPM)

## 5 special methods

[code](../14-220110/01-move-semantics/03-specials.cpp)

1. Copy constructor
    - Принимается l/rval.
2. Move
    - Если не реализован, то не используется. Если реализован и передали rval, то в приоритете.
    - other это lvalue внутри конструктора.
3. Operator=
4. Operator= с move
5. Destructor.

> Если реализован один, то значит хотим реализовать все. Если сами выделяем память, то нужно писать.

- Компилятор имеет право не вызывать конструктор для оптимизаци, если можно лишний раз не вызывать.
- Если в функции(create_foo) возвращается локальная переменная, то она автоматически rval и создается сразу в нужном
  месте что помогает избежать лишних мувов.

## Шаблоны начало

- До них были макросы.
  [базовый синтаксис](../14-220110/02-templates/02-template.cpp)
- На каждые типы создаются новые объекты.

### Детали

[code](../14-220110/02-templates/03-template.cpp)

- Не можем узнать является ли передавемый объект контейнером.
- Невызываемые static методы не компилируются.

### Пример на 5 специальных методов и шаблоны

[example_my_ptr](../14-220110/02-templates/04-shortcut.cpp)

- `std::exchange` - возвращает первый и в первый присваивает второй. Чтобы мувать мы присваиваем нам буфер другого, а ему наш пустой буфер.
- Внутри можно не писать `<T>` снаружи нужно писать.

## Вызов функторов (и функций) через шаблонную функцию

[example](../14-220110/03-functors/01-calling.cpp)
Вместо полиморфизма. Сгенерируется три разных функции apply. Нужно, чтобы было понятно, что вызывать, поэтому с
перегрузками функций плохо.
[copy_problem](../14-220110/03-functors/02-calling-stateful.cpp)

- Шаблон принимает по значенью, т.е копирует функцию передаваемый аргумент и не меняет его. Если хочется, чтобы по
  ссылке, то можно взять ref-wrapper (std::ref).
- Можно было написать `apply<Print&>();`, тоже передали по ссылке, но автовывод не работает
- Обобщенный функтор: [code](../14-220110/03-functors/03-storing.cpp).

## Operator *, ->

- `*` обычно возвращает ссылку. [code](../14-220110/04-misc/01-operator-deref.cpp).
- `->` должен вернуть что-то у чего уже есть встроенный оператор ->. [code](../14-220110/04-misc/02-operator-arrow.cpp).

## Swap idiom

