# Lecture 10-211117

- [Lecture 10-211117](#lecture-10-211117)
  - [01-rtti](#01-rtti)
    - [01-typeid-basic](#01-typeid-basic)
      - [Применение к неполимопфным](#применение-к-неполимопфным)
      - [Применение к полиморфным](#применение-к-полиморфным)
    - [02-typeid_operations](#02-typeid_operations)
    - [03-dynamic-cast](#03-dynamic-cast)
    - [04-dynamic-cast-ref](#04-dynamic-cast-ref)
  - [02-extra. Мелочи про наследование.](#02-extra-мелочи-про-наследование)
    - [01-final для классов](#01-final-для-классов)
    - [02-final-methods](#02-final-methods)
    - [10-virtual-in-constructors](#10-virtual-in-constructors)
    - [11-pure-virtual-ctors](#11-pure-virtual-ctors)
    - [20-inheritance-visibility. Публичное/защищенное/приватное наследование.](#20-inheritance-visibility-публичноезащищенноеприватное-наследование)
      - [public](#public)
      - [protected](#protected)
      - [private](#private)
    - [21-inheritance-class](#21-inheritance-class)
    - [22-inheritance-noncopyable. Зачем нужно приватное наследование.](#22-inheritance-noncopyable-зачем-нужно-приватное-наследование)
  - [03-patterns](#03-patterns)
    - [01-circle-ellipse](#01-circle-ellipse)
    - [patterns. Банда четырех. Gamma et al.](#patterns-банда-четырех-gamma-et-al)
    - [02-strategy](#02-strategy)
    - [03-factory](#03-factory)
    - [10-no-visitor. Больно добавлять операции, бесплатно наследников.](#10-no-visitor-больно-добавлять-операции-бесплатно-наследников)
    - [11-visitor. Больно добавлять наследников, бесплатно операции.](#11-visitor-больно-добавлять-наследников-бесплатно-операции)
  - [04-shared-library. Динамические библиотеки.](#04-shared-library-динамические-библиотеки)
    - [Библиотеки бывают](#библиотеки-бывают)
      - [abstact_plugin.h](#abstact_pluginh)
      - [plugin_multiply.cpp -> *.dll](#plugin_multiplycpp---dll)
      - [main.cpp](#maincpp)
      - [CMake](#cmake)

## 01-rtti

* RTTI - информация о типах, доступная во время исполнения программы.

### 01-typeid-basic

* `const std::type_info &info_base = typeid(Base);` - тип объекта. Доступ к константной ссылке на оъект type_info (его нельзя копировать/перемещать).
* У этого объекта есть всякие методы, например `info_base.name()`, но нужна библиотека boost, чтобы привести в читаемый вид.

#### Применение к неполимопфным

* Полиморфный тип(класс) - у которого есть хотя бы одна виртуальная функция.
*  `typeid(2+2+foo())` - не от полиморфного типа, значит определяется на этапе компиляции, т.е foo не вызывается.

#### Применение к полиморфным

*  Посмотрит на какой объект реально указывает ссылка или указатель, вызовет все функции, которые передадутся.
*  Нужно брать typeid от разыменованного uniq_ptr: `typeid(*dptr).name()`.

### 02-typeid_operations

* Можно сравнивать ==, !=.
* Нельзя скопировать, положить в set, сравнивать на <>.
* Чтобы положить в set есть обертка `std::type_index t = info_base`. Порядок не гарантируется.
* Нельзя получить список полей. Нельзя проверить родственность типов.

### 03-dynamic-cast

* Кастует указатель/ссылку на родителя к указателю/ссылке на ребенка, если указатель на самом деле показывает на ребенка. Если не успешно, то nullptr, вместо UB.
* Проверяет является ли тот, кого кастуем полиморфным(иначе не скомпилируется).
* Проверяет является ли то к чему кастуем, предком того, кого кастуем.
```c++
//  Получили ссылку на b
const Derived *d1 = dynamic_cast<const Derived1*>(&b); 

```
* При проверке можно писать это прямо внутри if.

### 04-dynamic-cast-ref

- Можно кастовать не к указателю, а к ссылке, но ссылка не мб nullptr, поэтому программа либо работает, либо падает с исключением.
```c++
//  Получили ссылку на b
const Derived *d1 = dynamic_cast<const Derived1*>(b) ;// Тут нет & перед b.  
```

* RTTI требует поддержки компилятора и памяти в экзэшнике. Можно отключить поддержку ключом. В GCC `-fno-rtti` . Запретится dynamic_cast.

## 02-extra. Мелочи про наследование.

### 01-final для классов

* `struct Derived final : Base` от структуры больше нельзя наследоваться.

### 02-final-methods

* Можно дописать к виртуальным функциям, которые нельзы перезаписать.
* Слово `override` всегда неявно дописывет `virtual`. Но и `override` не обязательно писать, он только помогает ошибки ловить.
* Не надо писать `virtual fooo() final{}`. fooo может не быть в родителе, и мы ее создадим в ременке виртуальной и сразу запретим перезаписывать.

### 10-virtual-in-constructors

* Всегда сначала вызывается конструктор Base, потом Derived. Поэтому если в конструкторе наследника вызываем переопределенную функцию, то она вызовется из родителя, т.к на момент исполнения конструктора объект Derived еще не создан. 
* Сначала вызывается деструктор Derived, потом Base.

### 11-pure-virtual-ctors

* Если вызываем в конструкторе Derived чисто виртуальную функцию родителя, то ошибка линковки.
* Эту функцию можно реализовать, специально для этого, но зачем?

### 20-inheritance-visibility. Публичное/защищенное/приватное наследование.

```c++
struct Derived1 : public Base {}; // Default for 'struct', the most popular.
struct Derived2 : protected Base {}; 
struct Derived3 : private Base {  // Default for 'class'.

```

#### public

- Ничего не изменилось

#### protected

- Все методы и поля Base защищенные в наследнике. Не можем их вызвать через наследника из наружи.
- Нельзя взять ссылку на базовый класс из наружи. Т.к тогда мы сможем вызвать функцию Base через эту ссылку.
- Есть доступ к этим полям из дальнейших наследников.
```c++
Derived2 d;
const Base &b=d2;
```

#### private

* Все поля приватные для наследника. Дальнейшие наследники (наследники наследника) не могут обратиться к полям и сконвертировать к ссылке на Base.
* Работает c-style cast, чтобы получить ссылку на Base, но не надо. 
 `:: Base & b2=(const Base&)*this`

* Просто Base написать в наследнике Derived3 тоже нельзя, т.к там не видно имя Base.

### 21-inheritance-class

* Наследование в классах по умолчанию приватное!

### 22-inheritance-noncopyable. Зачем нужно приватное наследование.

* В boost есть такая штука. Ее нельзя ни копировать, ни перемещать никаким образом. 
* От этой структуры можно унаследоваться(приватно) и тогда наследнику не будут генерироваться эти операторы.
* Наследоваться нужно приватно, чтобы нельзя было писать как в main. Совместимость по ссылкам с noncopyable не нужна.
* `noncopyable &n = f` - это не копирование, а создание ссылки. Берем из f кусок, который noncopyable.
```c++
struct noncopyable {  // boost::noncopyable

    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = delete;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = delete;

}; 

struct Foo : private noncopyable {}; 
int main() {

    Foo f;
    // noncopyable &n = f;  // WTF, 'private' prevents that.

    // Foo f2 = f; //- нельзя

}

```

## 03-patterns

### 01-circle-ellipse

- Логично наследовать круг от эллипса, но если в эллипсе есть методы set_width/height, то их никак нельзя переопределить в круге. 
- Т.е если есть методы для чтения, то круг от эллипса, если для записи, то эллипс от круга. Если и те и те, то никак.
- Функция для базового класса должна уметь работать с наследником.

### patterns. Банда четырех. Gamma et al.

- ООП - есть наследование, все функции виртуальные, нет ничего другого, например std::function.

### 02-strategy

- Создадим структуру AbstactTest. У нее полностью виртуальный метод run, переопределяется в самих тестах, например в TestSum).
- Можно создать вектор AbstactTest-ов, в который положить разные наследники.
- Так можно генерировать случайное число.
```c++
std::default_random_engine eng(std::random_device{}());
//random_device - медленный сложный генератор (шум с видеокарты)
// default_random_engine - генератор попроще от какого-то числа
std::bernoulli_distribution distrib; // Распределение Бернулли (1/0)
if (distrib(eng)) {}
```

* Т.е решения принимаются во время выполнения. Например массиву AbstractTest подсовываются разные в зависимости от рандома.
* Можно использовать std::function, лучше.
  

### 03-factory

* Есть структура StringFactory, которая генерирует строки. У нее полностью виртуальный метод `read_string`.
* От него можно унаследоваться и переопределить метод.
* Можем рандомно выбрать фабрику и присвоить ее к указателю на Abstract.
* Отличие от стратегии:  
там вызывали, чтобы что-то сделать с данными(что-то запустить), а здесь вызываем ради генерации нового значения.

### 10-no-visitor. Больно добавлять операции, бесплатно наследников.

* Хотим завести иерархию с наследниками, чтобы можно было делать операции с наследиками.
* Легко добавляем новых наследников. Просто переопределяем все виртуальные методы Base в нем.
* Нужно добавить новую операцию op3, не меняя предыдущий код. 
  + Для этого придется добавить кучу if с dynamic_cast, чтобы опреелить поведение в каждом наследнике.    
  + это работает за линейное время от количества наследников. 
  + Если появится еще наследник, то придется обновить код op3.

### 11-visitor. Больно добавлять наследников, бесплатно операции.
- `BaseVisitor` - базовый класс для операций над наследниками `Base`. Все виртуальное.
- У него есть метод visit для каждого наследника.
- Чтобы добавить op3, создаем структуру Op3, наследуюзуюся от BaseVisitor и перезаписываем методы. Вызов Op3 работает за константное время.
- Пока не можем написать `Op3.visit(&b)`, т.к на этапе компиляции не понятно на какой наследник указывает указатель Base*. 
- В `Base` создаем `virtual void accept(BaseVisitor &)=0`. В наследниках переопределяем (см. код). 
- Теперь мы бесплатно добавляем новые операции, но если добавляется наследник, то нужно поменять `BaseVisitor` и все реализованные Op до этого.
```c++
#include <iostream>
#include <memory>

struct BaseVisitor;
struct Base {
    virtual void op1() = 0;
    virtual void op2() = 0;

    virtual void accept(BaseVisitor &) = 0;
};

struct Derived1;
struct Derived2;

struct BaseVisitor {
    virtual void visit(Derived1 &d1) = 0;
    virtual void visit(Derived2 &d2) = 0;
};

struct Derived1 : Base {
    void op1() override {
        std::cout << "d1op1\n";
    }
    void op2() override {
        std::cout << "d1op2\n";
    }
    void accept(BaseVisitor &v) override {
        v.visit(*this);
    }
};

struct Derived2 : Base {
    void op1() override {
        std::cout << "d2op1\n";
    }
    void op2() override {
        std::cout << "d2op2\n";
    }
    void accept(BaseVisitor &v) override {
        v.visit(*this);
    }
};

struct Op3 : BaseVisitor {
    void visit(Derived1 &) override {
        std::cout << "d1op3\n";
    }
    void visit(Derived2 &) override {
        std::cout << "d2op3\n";
    }
};

int main() {
    // If we want to add Derived3, Derived4 we add a virtual function to all visitors. We are free to add new visitors (operations) without changing existing code.
    std::unique_ptr<Base> d1 = std::make_unique<Derived1>();
    std::unique_ptr<Base> d2 = std::make_unique<Derived2>();

    Op3 op3;
    // op3.visit(*d1);  // Should be resolved during compilation, unable.

    d1->op1();
    d1->op2();
    d1->accept(op3);
    d2->op1();
    d2->op2();
    d2->accept(op3);
}

```
- Можно [глянуть](https://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%81%D0%B5%D1%82%D0%B8%D1%82%D0%B5%D0%BB%D1%8C_(%D1%88%D0%B0%D0%B1%D0%BB%D0%BE%D0%BD_%D0%BF%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F) ).

## 04-shared-library. Динамические библиотеки.
- Библиотека:  
  программа состоит из нескольких единиц трансляции. Используем чужие.
### Библиотеки бывают
- **Статическая** - подключается на этапе компиляции. Код просто добавляется в наш экзэшник.
- **Динамические** - не компилируются в каждый экзэшник. В нашей программе говорим, что хотим взять такую-то функцию из такого-то файла dll.  
Чтобы мог запускать любой пользователь, то нужно все статически скопировать, либо все dll пользователю дать.

###
- Можно сделать экзэшник, который умеет подгружать dll во время выполнения.
- На низком уровне зависит от всего, поэтому нужно использовать библиотеку `boost::dll`, которая все умеет.
- plugin - тот, кто наследуется от этого класса.
#### abstact_plugin.h
```c++
#include <boost/config.hpp>
#include <string>

namespace plugins_demo {
class BOOST_SYMBOL_VISIBLE abstract_plugin { // Макрос нужен, чтобы класс можно было использовать как базовый для других dll.
public:
    virtual const std::string &name() const = 0;
    virtual float calculate(float x, float y) = 0;
    virtual ~abstract_plugin() = default;
};
}  // namespace plugins_demo
```
- Сам плагин:
#### plugin_multiply.cpp -> *.dll
```c++
#include <boost/config.hpp>
#include <iostream>
#include "abstract_plugin.h"

void foo();  // Not implemented anywhere

namespace plugin_multiply {
class plugin_multiply : public plugins_demo::abstract_plugin {
public:
    plugin_multiply() {
        std::cout << "Constructing plugin_multiply\n";
        // The following line will cause 'undefined reference' error.
        // foo();
        // Windows: linker error during DLL compilation.
        // Linux: loader error during .so loading in `main.cpp`, very similar to the linker. You may implement foo() in main.cpp.
        // macOS: linker error during .dylib compilation.
    }

    const std::string &name() const override {
        static std::string name = "multiply";
        return name;
    }

    float calculate(float x, float y) override {
        return x * y;
    }

    ~plugin_multiply() {
        std::cout << "Destructing plugin_multiply\n";
    }
};

extern "C" BOOST_SYMBOL_EXPORT plugin_multiply plugin; // Чтобы переменная стала видна в dll под именем plugin.
plugin_multiply plugin; // название plugin одинаково во всех плагинах. Нужно, т.к мы из dll можем подгрузить что-то конкретное(точное имя).
}  // namespace plugin_multiply
```
- Ключ `-shared`, чтобы показать, что компилируем именно *.dll, а не *.exe и *.o. Чтобы компилятор не требовал main.
  
- Сама программа:
- Компилируется: `g++ main.cpp -o main -lboost_filesystem-mt`
#### main.cpp
```c++
#include <boost/dll/import.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <vector>
#include "abstract_plugin.h"

#if BOOST_VERSION >= 107600 // Функция по-разному в разных версиях зовется
#define boost_dll_import_symbol ::boost::dll::import_symbol
#else
#define boost_dll_import_symbol ::boost::dll::import
#endif

namespace plugins_demo {
int main() {
    namespace fs = boost::filesystem; // Переименовываем нэймспэцс для удобства. Он нужен,чтобы открыть папку 'plugins/' далее.

    std::vector<boost::shared_ptr<abstract_plugin>> plugins;
    // f - имена файлов в папке plugins
    for (auto &f : fs::directory_iterator(fs::path("plugins/"))) {
        std::cout << "Loading from " << f << "\n";
        plugins.emplace_back(
            boost_dll_import_symbol<abstract_plugin>(f.path(), "plugin"));
            // Загрузи мне наследника класса abstract_plugin из библиотеки f.path() с названием plugin.
            // Вернет boost::shared_ptr на abstract_plugin.
        // TODO: dll::load_mode::append_decorations
    }

    for (auto &plugin : plugins) {
        std::cout << plugin->name()
                  << "->calculate(1.5, 1.5) == " << plugin->calculate(1.5, 1.5)
                  << std::endl;
    }
    return 0;
}
}  // namespace plugins_demo

int main() {
    return plugins_demo::main();
}
```
- `boost::shared_ptr` - хранит объект пока жив хоть один указатель.

#### CMake
```
cmake_minimum_required(VERSION 3.10)

project(shared-library CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)  # Recommended by Boost.DLL's manual

find_package(Boost 1.71 REQUIRED filesystem)  # Required by Boost.DLL
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Windows: libmultiply.dll (msys2), multiply.dll (VS)
# Linux: libmultiply.so
# macOS: libmultiply.dylib
add_library(multiply SHARED plugin_multiply.cpp)
add_library(sum SHARED plugin_sum.cpp)

add_executable(main main.cpp)
target_link_libraries(main ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
```
- Т.е можно брать классы с виртуальными функциями, от них наследоваться и наследников выносить в плагины.  
[hello](./09-211110/00b-special-operators.cpp)