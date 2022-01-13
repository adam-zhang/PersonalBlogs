# 右值引用
# 右值引用
## 背景
在C++11提出右值引用之前，C++03及更早的C++标准中，表达式的“值分类”（value categories）属性为左值或右值。左值是对应（refer to）内存中有确定存储地址的对象的表达式的值，而右值是所有不是左值的表达式的值。因而，右值可以是字面量、临时对象等表达式。能否被赋值不是区分C++左值与右值的依据，C++的const左值是不可赋值的；而作为临时对象的右值可能允许被赋值。左值与右值的根本区别在于是否允许取地址&运算符获得对应的内存地址。C++03及以前的标准定义了在表达式中左值到右值的三类隐式自动转换：

* 左值转化为右值；如整数变量i在表达式 （i+3）
* 数组名是常量左值，在表达式中转化为数组首元素的地址值
* 函数名是常量左值，在表达式中转化为函数的地址值

作为一种追求执行效率的语言，C++在用临时对象或函数返回值给左值对象赋值时的深度拷贝（deep copy）一直受到诟病。考虑到临时对象的生命期仅在表达式中持续，如果把临时对象的内容直接移动（move）给被赋值的左值对象，效率改善将是显著的。这就是移动语义的来源。

与传统的拷贝赋值运算符（copy assignment）成员函数、拷贝构造（copy ctor）成员函数对应，移动语义需要有移动赋值（move assignment）成员函数、移动构造（move ctor）成员函数的实现机制。可以通过函数重载来确定是调用拷贝语义还是移动语义的实现。

右值引用就是为了实现移动语义与完美转发所需要而设计出来的新的数据类型。右值引用的实例对应于临时对象；右值引用并区别于左值引用，用作形参时能重载辨识（overload resolution）是调用拷贝语义还是移动语义的函数。

无论是传统的左值引用还是C++11引进的右值引用，从编译后的反汇编层面上，都是对象的存储地址与自动解引用（dereference）。因此，右值引用与左值引用的变量都不能悬空(dangling)，也即定义时必须初始化从而绑定到一个对象上。右值引用变量绑定的对象，是编程者认为可以通过移动语义移走其内容的对象，对这种对象就需要定义为一种独特的值分类，即C++11标准称之为“临终值”（eXpire Value）。临终值对象既有存储地址因此可以绑定到右值引用变量上，而且它又是一个即将停止使用的对象可以被移走内容。所以临终值既不同于左值，也不同于传统的右值（C++11称之为纯右值），不能取地址运算（&）。另一方面，临终值兼有传统的左值与右值的性质：既对应于一个（临时）对象，称之为有标识（identity）；同时其内容可以移走，称之为可移动性（movability）。C++11标准把临终值与左值合称为广义左值，即指向某个物理存在的对象；把临终值与纯右值（对应C++03时的右值概念）合称为右值（C++11重新定义的概念），其内容可以移走（该右值生命期到此为止，此后将不再使用）。之所以称为右值而不叫做广义右值，是因为右值引用即可以与临终值对象绑定，也可以与纯右值对象绑定（这时往往自动生成一个临时对象）。

C++语言在引入了右值引用之后，面临着一个问题：如何让编程者指出哪个对象具有临终值？这有两种显式指定方法：如果函数（或运算符）的返回类型为右值引用，或者通过类型转换如static\_cast<Type&&>或者std::move()模板函数。

## 定义
设X为任何一种数据类型，则定义X&&是到数据类型X的右值引用（rvalue reference to X）。传统的引用类型X&被称为左值引用（lvalue reference to X）。例如：

```cpp
int i;
int &j = i; //傳統的左值引用數據類型的變量並初始化
int &&k = std::move(i);  //定義一個右值引用數據類型的變量並初始化。std::move定義於<utility>
```
## 语义
右值引用是一种数据类型，既有右值性质，也有左值性质。在C++11中为右值引用专门定义了临终值（eXpiring value）这一概念。右值引用很类似于传统的左值引用。

例如：

```cpp
int s=101;

int&& foo(){return static_cast<int&&>(s);} //返回值为右值引用

int main() 
{
   int i=foo();   //右值引用作为右值，在赋值运算符的右侧
   int&& j=foo(); //j是具名引用。因此运算符左侧的右值引用作为左值
   int* p=&j;     //取得j的内存地址
}
```
C++中，引用（reference）是指绑定到内存中的相应对象上。左值引用是绑定到左值对象上；右值引用是绑定到临时对象上。这里的左值对象是指可以通过取地址&运算符得到该对象的内存地址；而临时对象是不能用取地址&运算符获取到对象的内存地址。

C++11有如下引用绑定规则：

* 非常量左值引用（X& ）：只能绑定到X类型的左值对象；
* 常量左值引用（const X&）：可以绑定到 X、const X类型的左值对象，或X、const X类型的右值；\[注 8\]
* 非常量右值引用（X&&）：只能绑定到X类型的右值；
* 常量右值引用（const X&&）：可以绑定规定到X、const X类型的右值。
对上述引用绑定规则，值得注意的是：

1. 常量左值引用、常量右值引用、非常量右值引用，在绑定到字面量（literal）时，实际上用字面量构造了一个对象，然后绑定到该对象上；
上述引用绑定规定所提到的右值，包含了纯右值与临终值（xvalue）。即右值引用可以绑定到纯右值（隐式自动构造的临时对象），也可以绑定到临终值对象。
2. 绑定规则在调用重载函数，虚、实参数结合时起到决定作用，可以确定哪个版本的重载函数被调用。一些函数的形参为右值引用类型，因此可接受右值实参的绑定，如移动构造函数、移动赋值运算符、正常的成员函数如std::vector::push\_back；另一些函数的形参为左值引用类型，因此可接受左值实参的绑定。一些函数模板的形参是广义引用（universal reference，详见下文)，即可以接受左值对象绑定，也可以接受临时对象绑定，需要在模板推导时来决定形参是左值引用还是右值引用，这给了一套函数模板以极大地灵活性，可以同时处理两种引用类型，可以把函数参数的引用类型“完美转发”（详见下文）给被调用的实现函数。
3. C++0X曾经规定右值引用可以绑定到左值对象上，但在C++11中取消了这一许可。
4. 由于右值引用主要针对移动语义用来修改被引用的对象的内容，所以常量右值引用（const X&&）较少用到。

函数返回值是右值数据类型还是右值引用类型，区别在于前者是“传值”，后者是“传引用”可以修改被引用的对象。例如：

```cpp
#include <iostream>
#include <utility>

int i = 101, j = 101;

int foo(){ return i; }
int&& bar(){ return std::move(i); }
void set(int&& k){ k = 102; }
int main()
{
    foo();
    std::cout << i << std::endl;
    set(bar());
    std::cout << i << std::endl;      
}
```
上述例子中的函数set中可以对类型为右值引用的形参k赋值，这是因为C++标准规定，具名的右值引用被当作左值。这一规定的意义在于，右值引用本来是用于实现移动语义，因而需要绑定一个对象的内存地址，然后具有修改这一对象内容的权限，这些操作与左值绑定完全一样。右值绑定与左值绑定的分别在于函数重载时的行为。对于移动构造成员函数与移动赋值运算符成员函数，其形、实参数结合时是按照右值引用处理；而在这两个成语函数体内部，由于形参都是具名的，因而都被当作左值，这就可以用该形参来修改传入对象的内部状态。另外，右值引用作为xvalue（临终值）本来是用于移动语义中一次性搬空其内容。具名使其具有更为持久的生存期，这是危险的，因而规定具名后为左值引用，除非程序显式指定其类型强制转换为右值引用。

C++11标准给了用户更大的决定权，例如把左值或临终值，转化为右值引用。这是通过定义在C++标准程序库中的std::move实现的。std::move是个模板函数，把输入的左值或右值转换为右值引用类型的临终值。其核心是强制类型转换static\_cast<Type&&>()语句。

除了左值引用、右值引用，对于函数参数类型是到模板参数类型的右值引用的情形，称之为广义引用（universal reference）或转发引用（forward reference）。例如：

```cpp
 template<typename T> int foo(T&& param);
```
详见完美转发一节的讨论。

## 完美转发
完美转发也是C++11标准引入右值引用这一概念所要实现的目标之一。

### 背景
在C++程序设计中，一个常见的类工厂函数，如下例：

```cpp
template <typename T, typename Arg> 
shared_ptr<T> factory(Arg arg)
{
    return shared_ptr<T>( new T(arg) );
}
```
参数对象arg在上例中是传值方式传递，这带来了生成额外的临时对象的代价。对于类工厂函数，完美的参数传递应该是引用方式传递。因而，在boost:bind中，参数是左值引用：

```cpp
template <typename T, typename Arg> 
shared_ptr<T> factory(Arg& arg)
{
    return shared_ptr<T>( new T(arg) );
}
```
这种实现的问题是形参不能绑定右值实参。如factory(102)将编译报错。进一步解决办法是按常量引用方式传递参数，如下例：

```cpp
template <typename T, typename Arg> 
shared_ptr<T> factory(const Arg& arg)
{
    return shared_ptr<T>( new T(arg) );
}
```
这种实现的问题是不能支持移动语义。

形参使用右值引用可以解决完美转发问题。

### 引用折叠规则
对于C++语言，不可以在源程序中直接对引用类型再施加引用。T& &将编译报错。C++11标准中仍然禁止上述显式对引用类型再施加引用，但如果在上下文环境中（包括模板实例化、typedef、auto类型推断等）如出现了对引用类型再施加引用，则施行引用塌缩规则（reference collapsing rule）\[注 10\]：

* T& &变为T&
* T& &&变为T&
* T&& &变为T&
* T&& &&变为T&&

### 模板参数类型推导
对函数模板templatevoid foo(T&&);，应用上述引用塌缩规则，可推导出如下结论：

* 如果实参是类型A的左值，则模板参数T的类型为A&，形参类型为A&；
* 如果实参是类型A的右值，则模板参数T的类型为A&&，形参类型为A&&。
这还适用于类模板的成员函数模板的类型推导：

```cpp
template <class T > class vector {
  public: 
  void push_back(T&& x); // T是类模板参数 ⇒ 该成员函数不需要类型推导;这里的函数参数类型就是T的右值引用
   template <class Args> void emplace_back(Args&& args); //  该成员函数是个函数模板，有自己的模板参数，需要类型推导
};
```
函数模板的形参必须是T&&形式，才需要模板参数类型推导。即使形参声明为const T&&形式，就只能按字面意义使用，不需要模板参数类型推导。

```cpp
template<typename T>void f(const T&& param); // 这里的“&&”不需要类型推导，意味着“常量类型T的右值引用”
template<typename T>void f(std::vector<T>&& param);  // 这里的“&&”不需要类型推导，意味着std::vector<T>的右值引用
```
### 完美转发的解决方案
```cpp
template <typename T, typename Arg> 
shared_ptr<T> factory(Arg&& arg)
{
  return shared_ptr<T>( new T(std::forward<Arg>(arg) ) );
}
```
其中std::forward是定义在C++标准程序库中的模板函数：

```cpp
template< class T > T&& forward( typename std::remove_reference<T>::type& t )
{
return static_cast<T&&>(t);
}
template< class T > T&& forward( typename std::remove_reference<T>::type&& t )
{
return static_cast<T&&>(t);
}
```
std::forward在调用时，应当显示给出该函数模板参数类型。其用途是：如果函数forward的实参的数据类型是左值引用，则返回类型为左值引用；如果函数forward的实参的数据类型是右值引用，则返回类型为右值引用，返回值的分类属于临终值，从而把参数的信息完整地传递给下一级被调用的函数。从上述std::forward的定义实现来看，实参必须是个为左值的引用对象，但是实参的数据类型有两种可能：

* 实参的数据类型S是左值引用类型，std::forward的返回类型S&&根据引用塌缩规则变为S&，即返回值仍为左值引用类型；
* 实参的数据类型S是右值引用类型（这是因为右值引用类型的具名变量实际上表现为左值），std::forward的返回类型S&&根据引用塌缩规则变为S&&，即返回值为右值引用类型。
类似的，std::move也是利用上述模板函数类型推导规则，定义在C++标准程序库中，无论输入的实参是左值还是右值，均返回右值引用：

```cpp
template<class T>
typename std::remove_reference<T>::type&& move(T&& a) noexcept
{ 
return static_cast<typename std::remove_reference<T>::type&&>(a);
}
```
定义在C++标准程序库中std::remove\_reference是个类模板，其中定义的类型type是引用的基类型:

```cpp
template< class T > struct remove_reference      {typedef T type;};
template< class T > struct remove_reference<T&>  {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};
```
## 移动语义与异常
移动语义只是把资源从一个右值对象搬移到被构造或者被赋值对象内部，因此保证不抛出异常是容易实现的。在std::vector这样的容器内实现移动语义，必须显式声明容器元素类的移动构造成员函数、移动赋值运算符成员函数不抛出异常。

## 右值引用的类型推导
C++中涉及到的右值引用的类型推导，除了上述模板参数类型推导，还有：

### auto关键字的类型完美转发
C++11使用auto声明变量时，如：auto&& var=initValue;“auto&&”并不意味着这一定是右值引用类型的变量，而是类似于模板函数参数的类型推导，既可能是左值引用，也可能是右值引用。其目的是把初始化表达式的值分类情况，完美转发给由auto声明的变量。也即：

* 如果初始化值（initializer）是类型A的左值，则声明的变量类型为左值引用A&；
* 如果初始化值是类型A的右值，则声明的变量类型为右值引用A&&。

```cpp
Type1&& var1=anotherType1Instance; // var1的类型是右值引用，但是作为左值
auto&& var2=var1;       //var2的类型是左值引用
std::vector<int> v;
auto&& val = v[0]; // std::vector::operator[]的返回值是元素左值，所以val的类型是左值引用
Widget makeWidget(); // 类工厂函数
Widget&& var1 = makeWidget() // var1的类型是右值引用，具有左值。
Widget var2 = static_cast<Widget&&>(var1); // var2在初始化时可以使用移动构造函数。
```
### typedef的类型推导
typedef也可能会用到引用塌缩规则。例如：

```cpp
template<typename T>    class Widget {
    typedef T& LvalueRefType;

};
Widget<int&&> w; // LvalueRefType的类型为int&
void f(Widget<int&>::LvalueRefType&& param); //param的类型为int&
```
### decltype类型推导
decltype也可能会用到引用塌缩规则。例如：

```cpp
int var;
decltype(var)&& v1=std::move(var); //类型是int&&
```



