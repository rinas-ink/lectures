# Lecture 04-210923 (часть)
- [Lecture 04-210923 (часть)](#lecture-04-210923-часть)
  - [Конструктор](#конструктор)
  - [Преобразования и конструкторы](#преобразования-и-конструкторы)
  - [Операторы](#операторы)
    - [== (code)](#-code)
    - [++ (code)](#-code-1)
    - [+=, + (code)](#--code)
  - [Продление жизни при помощи const](#продление-жизни-при-помощи-const)
    - [Висячая ссылка (code) После выхода из фунции вектор удаляется и ссылка начинает указываь не несуществующую память.](#висячая-ссылка-code-после-выхода-из-фунции-вектор-удаляется-и-ссылка-начинает-указываь-не-несуществующую-память)
    - [Умирающий вектор и оживление const (code)](#умирающий-вектор-и-оживление-const-code)
    - [Проблемы с неодноходовым оживлением](#проблемы-с-неодноходовым-оживлением)
## Конструктор

[bad](../04-210923/02-ctors/03a-bad.cpp), [good](../04-210923/02-ctors/03b-member-initialization-list.cpp)
Нужно делать `Bigint():values{1,2,3}{};`, т.к тогда values один раз конструируется. Если писать в теле конструктора, то
сначала сконструируется по дефолту, а потом перезапишется.

- Когда переопределяем конструктор дефолтный удаляется.

## Преобразования и конструкторы

([code](../04-210923/03-conversions/03b-mix.cpp))  Здесь не понятно вызывать у f `operator Bar()` или у b конструктор от `Foo` => Ошибка компиляции.

```c++
struct Bar;

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

int main() {
    Foo f;
    Bar b = f;  // ambiguous (неоднозначно)
}
```

## Операторы

### == ([code](../04-210923/05-operators/01c-friends.cpp))
  Хочется чтобы было симметричным, т.к мы хотим сравнить и `int==bigint` и `bigint==int`. У `bigint` можем определить
  implicit ctor, у int - нет. Хочется сравнивать приватные поля не раскрывая их миру. Поэтому делаем френд функцию.
### ++ ([code](../04-210923/05-operators/02-increment.cpp))

```c++
struct myint {
    int x
    bigint &operator++() {
        x+=1;
        return *this;
    }
    bigint operator++(int) {
        myint t=x;
        x+=1;
        return t;
    }
};
int main() {
    myint a = 4;
    ++a;  // a.operator++()
    a++;  // a.operator++(0)
} 
```

### +=, + ([code](../04-210923/05-operators/03-increment.cpp))

```c++
struct bigint {
    ...
    bigint operator+(const bigint &other) const {
        return bigint(*this) += other;
    }
    
    bigint &operator+=(const bigint &other) {
        return *this = *this + other;
    }// Т.к хотим так (b+=1)+=1
};
```

## Продление жизни при помощи const
### Висячая ссылка ([code](../04-210923/06-const-wtf/01-dangling.cpp)) После выхода из фунции вектор удаляется и ссылка начинает указываь не несуществующую память.
```c++
std::vector<int>& foo() {
    std::vector<int> vec{1, 2, 3};
    return vec;
}

int main() {
    std::vector<int> vec = foo();
    std::cout << vec.size() << "\n";
}
```
### Умирающий вектор и оживление const ([code](../04-210923/06-const-wtf/02-lifetime.cpp))
- Временный вектор умер в момент `;`, поэтому ссылка тоже на несуществующий объект. `std::vector<int> &vec = std::vector{1, 2, 3};`. Это кажется даже не скомпилируется.
- Как чинить? Взять константную ссылку. Тогда у временного объекта продлевается время жизни, чтобы соответствовать этой константной ссылке. Это сделано, чтобы можно в такие функции передавать.
```c++
void println(const std::vector<int> &) {}
int main() {
    // https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/
    const std::vector<int> &vec = std::vector{1, 2, 3};
    println(std::vector{1, 2, 3});
}
```
### Проблемы с неодноходовым оживлением 
([one](../04-210923/06-const-wtf/03-id.cpp))
- Возврат ссылок из функций - зло.
```c++
int id_val(int x) {
    return x;
}

const int &id_ref(const int &x) {
    return x; //Время жизни до сюда продлилось...
}

int main() {
    { // Тут все ок
        const int &x = id_val(10);
        std::cout << x << "\n";
    }
    {//...А до сюда уже не продлится, т.к непонятно, где лежит объект справа
        const int &x = id_ref(10);
        std::cout << x << "\n";
    }
}
```
([two](../04-210923/06-const-wtf/04-min.cpp))
- Время жизни не продлевается т.к min возвращает константную ссылку(см. предыдущий пункт).
```c++
// Rvalue lifetime disaster, Arno Schoedl.
const auto &val = std::min(3, 4); 
std::cout << val << "\n";
```