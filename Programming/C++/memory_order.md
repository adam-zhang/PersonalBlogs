# c++11：std::memory_order（c++11 六种内存序）

仅靠原子指令实现不了对资源的访问控制。这造成的原因是编译器和cpu实施了重排指令，导致读写顺序会发生变化，只要不存在依赖，代码中后面的指令可能会被放在前面，从而先执行它。cpu这么做是为了尽量塞满每个时钟周期，在单位时间内尽量执行更多的指令，从而提高吞吐率。

我们先来看两个线程代码
```
// thread 1
// flag初始值为false
a.init();
flag= true;

// thread 2
if(flag) {
 a.bar();
}
```
线程2在ready为true的时候会访问p，对线程1来说，如果按照正常的执行顺序，那么p先被初始化，然后在将ready赋为true。但对多核的机器而言，情况可能有所变化：

线程1中的ready = true可能会被cpu或编译器重排到p.init()的前面，从而优先执行ready = true这条指令。在线程2中，p.bar()中的一些代码可能被重排到if(ready)之前。
即使没有重排，ready和p的值也会独立地同步到线程2所在核心的cache，线程2仍然可能在看到ready为true时看到未初始化的p。
为了解决这个问题，我们一种解决方法是使用互斥元，还有一种就是cpu和编译器提供了memory fence，让用户可以声明访存指令的可见性关系，c++11总结为以下memory order：

头文件： #include<atomic>
```
typedef enum 

{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst

} memory_order;	 	(C++11 起)
(C++20 前)
enum class memory_order : /*unspecified*/ {
    relaxed, consume, acquire, release, acq_rel, seq_cst
};
inline constexpr memory_order memory_order_relaxed = memory_order::relaxed;
inline constexpr memory_order memory_order_consume = memory_order::consume;
inline constexpr memory_order memory_order_acquire = memory_order::acquire;
inline constexpr memory_order memory_order_release = memory_order::release;
inline constexpr memory_order memory_order_acq_rel = memory_order::acq_rel;

inline constexpr memory_order memory_order_seq_cst = memory_order::seq_cst;	 	(C++20 起)
memory order	作用
memory_order_relaxed	无fencing作用，cpu和编译器可以重排指令
memory_order_consume	后面依赖此原子变量的访存指令勿重排至此条指令之前
memory_order_acquire	后面访存指令勿重排至此条指令之前
memory_order_release	前面的访存指令勿排到此条指令之后。当此条指令的结果被同步到其他核的cache中时，保证前面的指令也已经被同步。
memory_order_acq_rel	acquare + release
memory_order_seq_cst	acq_rel + 所有使用seq_cst的指令有严格的全序关系
```
有了memory_order，我们可以这么改上面的例子：
```
// thread 1
// flag初始值为false
a.init();
flag.store(true, std::memory_order_release);  //前面的不能在我后面执行
 

// thread 2
if(flag.load(std::memory_order_acquire)) {   //后面的不能在我前面执行
 a.bar();
}
```
这样就保证了线程1和线程2的顺序性，比如线程2在看到flag==true时，能看到线程1 realse之前所有操作。 也就保证了代码符合我们的预期。

注意，memory fence（内存栅栏）不等于可见性，即使线程2恰好在线程1在把flag设置为true后读取了flag也不意味着它能看到true，因为同步cache是有延时的。memory fence保证的是可见性的顺序：“假如我看到了a的最新值，那么我一定也得看到b的最新值”。
