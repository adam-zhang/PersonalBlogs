# c/c++内存泄漏检测工具

## 1. VC自带的CRT:_CrtCheckMemory   调试器和 CRT 调试堆函数
### 用法
``````
/************************************************************************
*   环境：VC6.0
*   程序功能：CRT 检测内存泄漏
    
*   创建：  2014/3/20
*
*   版本号：1.0
************************************************************************/
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>  
#include <stdio.h>  
using namespace std;   
int main(){  
int *p=(int*)malloc(sizeof(int));
*p=6;
cout<<*p<<endl;
free(p);
p=NULL;
_CrtDumpMemoryLeaks();//注意这条语句的摆放位置，因为这会直接影响到你的判断结果
    return 0;  
}  
``````
F5运行即可

### 原理
在使用Debug版的malloc分配内存时，malloc会在内存块的头中记录分配该内存的文件名及行号。当程序退出时CRT会在main()函数返回之后做一些清理工作，这个时候来检查调试堆内存，如果仍然有内存没有被释放，则一定是存在内存泄漏。从这些没有被释放的内存块的头中，就可以获得文件名及行号。
缺点
只能检测出内存泄漏及其泄漏点的文件名和行号，但是并不知道泄漏究竟是如何发生的，并不知道该内存分配语句是如何被执行到的。
CRT的使用详见：http://blog.sina.com.cn/s/blog_4012aec7010085jo.html

附加：任务管理器---->性能------>内存

若运行程序时，内存使用一直增加，则程序发生内存泄漏。

--------------------------------------------------------------------------------------------------------------------
## 2.bounds checker
### 简介
BoundsChecker 是一个Run-Time错误检测工具，它主要定位程序在运行时期发生的各种错误。BoundsChecker能检测的错误包括：

1. 指针操作和内存、资源泄露错误，比如：

1.1 内存泄露；

内存操作方面的错误，比如：

   内存读、写溢出；

   使用未初始化的内存。

1.2 资源泄露；

1.3 对指针变量的错误操作。

### 安装
先安装BoundsChecker软件，安装完成之后，会在VC++6.0环境中有一个栏目的标题就是BoundsChecker。
使用
使用BoundsChecker对程序的运行时错误进行检测，有两种使用模式可供选择。一种模式叫做ActiveCheck，一种模式叫做FinalCheck。下面分别进行介绍。
ActiveCheck
ActiveCheck是BoundsChecker提供的一种方便、快捷的错误检测模式，它能检测的错误种类有限，只包括：内存泄露错误、资源泄露错误、API函数使用错误。

要想使用ActiveCheck模式来检测程序的运行时错误，只需在VC++集成开发环境中打开BoundsChecker功能，然后从调试状态运行程序即可。此时ActiveCheck会在后台自动运行，随时检测程序是否发生了错误。

FinalCheck
FinalCheck具有BoundsChecker提供的所有检错功能。FinalCheck 是ActiveCheck的超集，它除了能够检测出ActiveCheck能够检测出的错误，还能发现很多 ActiveCheck 不能检测到的错误，包括：指针操作错误、内存操作溢出、使用未初始化的内存等等，并且，对于ActiveCheck能检测出的错误，FinalCheck能够给出关于错误更详细的信息。所以，我们可以把FinalCheck认为是ActiveCheck的功能增强版。我们付出的代价是：程序的运行速度会变慢，有时甚至会变的很慢。

要想在FinalCheck 模式下测试程序，不能使用VC++集成开发环境提供的编译连接器来构造程序，而必须要使用BoundsChecker提供的编译连接器来编译连接程序。当 BoundsChecker的编译连接器编译连接程序时，会向程序中插装一些错误检测代码，这也就是FinalCheck能够比ActiveCheck找 到更多错误的原因。


## 3. Visual Leak Detector
### 简介

Visual Leak Detector是一款用于Visual C++的免费的内存泄露检测工具。可以在http://www.codeproject.com/tools/visualleakdetector.asp 下载到。相比较其它的内存泄露检测工具，它在检测到内存泄漏的同时，还具有如下特点：

1.  可以得到内存泄漏点的调用堆栈，如果可以的话，还可以得到其所在文件及行号；
2.  可以得到泄露内存的完整数据；
3.  可以设置内存泄露报告的级别；
4.  它是一个已经打包的lib，使用时无须编译它的源代码。而对于使用者自己的代码，也只需要做很小的改动；
5.  他的源代码使用GNU许可发布，并有详尽的文档及注释。对于想深入了解堆内存管理的读者，是一个不错的选择。

### 使用
    安装：首先从网站上下载zip包，解压之后得到vld.h, vldapi.h, vld.lib, vldmt.lib, vldmtdll.lib, dbghelp.dll等文件。将.h文件拷贝到Visual C++的默认include目录下，将.lib文件拷贝到Visual C++的默认lib目录下，便安装完成了。因为版本问题，如果使用windows 2000或者以前的版本，需要将dbghelp.dll拷贝到你的程序的运行目录下，或其他可以引用到的目录。

``````
/************************************************************************
*   环境：VC6.0
*   程序功能：使用isual Leak Detector 检测内存泄漏
    
*   创建：  2014/3/28
*
*   版本号：1.0
************************************************************************/
#include <vld.h>
#include <stdlib.h>
#include <stdio.h>


void f()
{
    int *p = new int(0x12345678);
    printf("p=%08x, ", p);
//delete p;
}

int main() 


{  
    f();
   return 0;
}  
``````
编译运行后，在标准输出窗口得到：
``````
p=003a89c0

在Visual C++的Output窗口得到：

WARNING: Visual Leak Detector detected memory leaks!

---------- Block 57 at 0x003A89C0: 4 bytes ----------  --57号块0x003A89C0地址泄漏了4个字节

  Call Stack:                                               --下面是调用堆栈

    d:\test\testvldconsole\testvldconsole\main.cpp (7): f  --表示在main.cpp第7行的f()函数

    d:\test\testvldconsole\testvldconsole\main.cpp (14): main –双击以引导至对应代码处

    f:\rtm\vctools\crt_bld\self_x86\crt\src\crtexe.c (586): __tmainCRTStartup

    f:\rtm\vctools\crt_bld\self_x86\crt\src\crtexe.c (403): mainCRTStartup

    0x7C816D4F (File and line number not available): RegisterWaitForInputIdle

  Data:                                   --这是泄漏内存的内容，0x12345678

    78 56 34 12                                                  xV4..... ........

Visual Leak Detector detected 1 memory leak.    
``````
第二行表示57号块有4字节的内存泄漏，地址为0x003A89C0，根据程序控制台的输出，可以知道，该地址为指针p。程序的第7行，f()函数里，在该地址处分配了4字节的堆内存空间，并赋值为0x12345678，这样在报告中，我们看到了这4字节同样的内容。

对于每一个内存泄漏，这个报告列出了它的泄漏点、长度、分配该内存时的调用堆栈、和泄露内存的内容（分别以16进制和文本格式列出）。

双击该堆栈报告的某一行，会自动在代码编辑器中跳到其所指文件的对应行。这些信息对于我们查找内存泄露将有很大的帮助。
### Visual Leak Detector工作原理
下面让我们来看一下该工具的工作原理。

    在这之前，我们先来看一下Visual C++内置的内存泄漏检测工具是如何工作的。Visual C++内置的工具CRT Debug Heap工作原来很简单。在使用Debug版的malloc分配内存时，malloc会在内存块的头中记录分配该内存的文件名及行号。当程序退出时CRT会在main()函数返回之后做一些清理工作，这个时候来检查调试堆内存，如果仍然有内存没有被释放，则一定是存在内存泄漏。从这些没有被释放的内存块的头中，就可以获得文件名及行号。

    这种静态的方法可以检测出内存泄漏及其泄漏点的文件名和行号，但是并不知道泄漏究竟是如何发生的，并不知道该内存分配语句是如何被执行到的。要想了解这些，就必须要对程序的内存分配过程进行动态跟踪。Visual Leak Detector就是这样做的。它在每次内存分配时将其上下文记录下来，当程序退出时，对于检测到的内存泄漏，查找其记录下来的上下文信息，并将其转换成报告输出。

#### 初始化
    Visual Leak Detector要记录每一次的内存分配，而它是如何监视内存分配的呢？Windows提供了分配钩子(allocation hooks)来监视调试堆内存的分配。它是一个用户定义的回调函数，在每次从调试堆分配内存之前被调用。在初始化时，Visual Leak Detector使用_CrtSetAllocHook注册这个钩子函数，这样就可以监视从此之后所有的堆内存分配了。

    如何保证在Visual Leak Detector初始化之前没有堆内存分配呢？全局变量是在程序启动时就初始化的，如果将Visual Leak Detector作为一个全局变量，就可以随程序一起启动。但是C/C++并没有约定全局变量之间的初始化顺序，如果其它全局变量的构造函数中有堆内存分配，则可能无法检测到。Visual Leak Detector使用了C/C++提供的#pragma init_seg来在某种程度上减少其它全局变量在其之前初始化的概率。根据#pragma init_seg的定义，全局变量的初始化分三个阶段：首先是compiler段，一般c语言的运行时库在这个时候初始化；然后是lib段，一般用于第三方的类库的初始化等；最后是user段，大部分的初始化都在这个阶段进行。Visual Leak Detector将其初始化设置在compiler段，从而使得它在绝大多数全局变量和几乎所有的用户定义的全局变量之前初始化。

#### 记录内存分配
       一个分配钩子函数需要具有如下的形式：
int YourAllocHook( int allocType, void *userData, size_t size, int blockType, long requestNumber, const unsigned char *filename, int lineNumber);
    就像前面说的，它在Visual Leak Detector初始化时被注册，每次从调试堆分配内存之前被调用。这个函数需要处理的事情是记录下此时的调用堆栈和此次堆内存分配的唯一标识——requestNumber。
    得到当前的堆栈的二进制表示并不是一件很复杂的事情，但是因为不同体系结构、不同编译器、不同的函数调用约定所产生的堆栈内容略有不同，要解释堆栈并得到 整个函数调用过程略显复杂。不过windows提供一个StackWalk64函数，可以获得堆栈的内容。StackWalk64的声明如下：
``````
BOOL StackWalk64(
DWORD MachineType,
HANDLE hProcess,
HANDLE hThread,
LPSTACKFRAME64 StackFrame,
PVOID ContextRecord,
PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress
);
STACKFRAME64结构表示了堆栈中的一个frame。给出初始的STACKFRAME64，反复调用该函数，便可以得到内存分配点的调用堆栈了。
    // Walk the stack.
    while (count < _VLD_maxtraceframes) {
        count++;
        if (!pStackWalk64(architecture, m_process, m_thread, &frame, &context,
                          NULL, pSymFunctionTableAccess64, pSymGetModuleBase64, NULL)) {
            // Couldn't trace back through any more frames.
            break;
        }
        if (frame.AddrFrame.Offset == 0) {
            // End of stack.
            break;
        }

        // Push this frame's program counter onto the provided CallStack.
        callstack->push_back((DWORD_PTR)frame.AddrPC.Offset);
    }
``````
       那么，如何得到初始的STACKFRAME64结构呢？在STACKFRAME64结构中，其他的信息都比较容易获得，而当前的程序计数器(EIP)在 x86体系结构中无法通过软件的方法直接读取。Visual Leak Detector使用了一种方法来获得当前的程序计数器。首先，它调用一个函数，则这个函数的返回地址就是当前的程序计数器，而函数的返回地址可以很容易 的从堆栈中拿到。下面是Visual Leak Detector获得当前程序计数器的程序：
``````
#if defined(_M_IX86) || defined(_M_X64)

#pragma auto_inline(off)
DWORD_PTR VisualLeakDetector::getprogramcounterx86x64 ()
{
    DWORD_PTR programcounter;

    __asm mov AXREG, [BPREG + SIZEOFPTR] // Get the return address out of the current stack frame
    __asm mov [programcounter], AXREG    // Put the return address into the variable we'll return

    return programcounter;
}
#pragma auto_inline(on)
#endif // defined(_M_IX86) || defined(_M_X64)
``````
       得到了调用堆栈，自然要记录下来。Visual Leak Detector使用一个类似map的数据结构来记录该信息。这样可以方便的从requestNumber查找到其调用堆栈。分配钩子函数的 allocType参数表示此次堆内存分配的类型，包括_HOOK_ALLOC, _HOOK_REALLOC, 和 _HOOK_FREE，下面代码是Visual Leak Detector对各种情况的处理。
``````
    switch (type) {
    case _HOOK_ALLOC:
        visualleakdetector.hookmalloc(request);
        break;

    case _HOOK_FREE:
        visualleakdetector.hookfree(pdata);
        break;

    case _HOOK_REALLOC:
        visualleakdetector.hookrealloc(pdata, request);
        break;

    default:
        visualleakdetector.report("WARNING: Visual Leak Detector: in allochook(): Unhandled allocation type (%d)./n", type);
        break;
    }
``````
这 里，hookmalloc()函数得到当前堆栈，并将当前堆栈与requestNumber加入到类似map的数据结构中。hookfree()函数从类 似map的数据结构中删除该信息。hookrealloc()函数依次调用了hookfree()和hookmalloc()。
#### 总之：
    钩子函数在Visual Leak Detector初始化时被注册，每次从调试堆分配内存之前被调用。这个函数需要处理的事情是记录下此时的调用堆栈和此次堆内存分配的唯一标识。即钩子哈思楠记录并保存内存分配的堆栈情况。
检测内存泄露
    前面提到了Visual C++内置的内存泄漏检测工具的工作原理。 与该原理相同，因为全局变量以构造的相反顺序析构，在Visual Leak Detector析构时，几乎所有的其他变量都已经析构，此时如果仍然有未释放之堆内存，则必为内存泄漏。
    分配的堆内存是通过一个链表来组织的，检查内存泄漏则是检查此链表。但是windows没有提供方法来访问这个链表。Visual Leak Detector使用了一个小技巧来得到它。首先在堆上申请一块临时内存，则该内存的地址可以转换成指向一个_CrtMemBlockHeader结构， 在此结构中就可以获得这个链表。代码如下：
``````
    char *pheap = new char;
    _CrtMemBlockHeader *pheader = pHdr(pheap)->pBlockHeaderNext;
delete pheap;
``````
其中pheader则为链表首指针。

报告生成
        前面讲了Visual Leak Detector如何检测、记录内存泄漏及其其调用堆栈。但是如果要这个信息对程序员有用的话，必须转换成可读的形式。Visual Leak Detector使用SymGetLineFromAddr64()及SymFromAddr()生成可读的报告。
``````
            // Iterate through each frame in the call stack.
            for (frame = 0; frame < callstack->size(); frame++) {
                // Try to get the source file and line number associated with
                // this program counter address.
                if (pSymGetLineFromAddr64(m_process,
                   (*callstack)[frame], &displacement, &sourceinfo)) {
                    ...
                }

                // Try to get the name of the function containing this program
                // counter address.
                if (pSymFromAddr(m_process, (*callstack)[frame],
                    &displacement64, pfunctioninfo)) {
                    functionname = pfunctioninfo->Name;
                }
                else {
                    functionname = "(Function name unavailable)";
                }
                ...
            }
``````
#### 概括讲来
Visual Leak Detector的工作分为3步，
1. 首先在初始化注册一个钩子函数；
2. 然后在内存分配时该钩子函数被调用以记录下当时的现场；
3. 最后检查堆内存分配链表以确定是 否存在内存泄漏并将泄漏内存的现场转换成可读的形式输出。


## 附加

linux下内存泄漏检测工具 Valgrind memcheck

linux调试工具MEMWATC
## VC++6.0调试：Watch窗口的使用
``````
/*************************************************************************  

      环境：VC6.0
*   程序功能：使用watch窗口调试程序
   
*   创建：  2014/3/20
*
*   版本号：1.0
************************************************************************/
#include <stdio.h>  
#include <windows.h>  
 
class AutoExpand 
{ 
public: 
    AutoExpand(int val, char* pval) 
   { 
        a = val; 
        p = pval; 
    } 
private: 
    int a; 
    char *p; 
}; 
class CantExpand 
{ 
public: 
    CantExpand(int val, char* pval) 
    { 
        a = val; 
        p = pval; 
    } 
private: 
    int a; 
    char *p; 
}; 
 
int main(void) 
{ 
    int p[4] = {0x31,0x32,0x33,0x34}; 
    int *a  = p; 
     
    FILE* fp = fopen("File Not Exist", "r"); 
    DWORD dwError = GetLastError(); 
     
    AutoExpand ae(10, "abc"); 
    CantExpand ce(10, "abc"); 
 
    return 0; 
} 
``````
上面代码中出现的变量先说明一下：

　　p: 是整形数组，含四个元素，总共16Byte；
　　a: 整形指针，指向数组p；
　　fp: 文件指针，用来标识打开的"File Not Exist"，我机器里是没这个文件的；
　　dwError: 获得fopen失败的错误码。一般来说可以用FormatMessge来格式化这个错误原因或者直接用VC自带的工具errorlookup来查找这个错误码的解释；
　　ae和ce: 是自定义的AutoExpand类型的变量和CantExpand类型的变量。注意，这两种类型只有类型名字不同。

　　下面看一下我在调试这个程序的时候，watch窗口的显示：
``````
View---->debug windows--->watch,call stack,memory,register
``````

上图中，左边是Context窗口的Locals页，显示所有局部变量。右边是Watch窗口，是我自己添加的要观察的对象。　

　　好，先看看整形数组p。我们看到Context窗口的显示p其实只显示了数组的地址，点了+号展开后，显示出了4个整形数据。而右边窗口，我添加了一个p,c。p后面加个",c"是什么意思呢？看看效果，p,c下面的[0]，[1]，[2]，[3]显示的是这4个整形值对应的ascii字符哈。所以从这里有了第一个小技巧：

1. watch窗口中，在整形变量后面加上",c"可以显示该变量对应的ASCII字符。实际上，可以直接敲数字这么显示也行。比如上面右边窗口中的118,c的对应值是'v'。也就是说118对应的ASCII字符是'v'。那么，反过来，要知道一个字符的ASCII码值怎么看呢？看上面，'v',d就是显示字符'v'对应的十进制ASCII码值是118。 'v'，x显示的是对应的十六进制的ASCII码值。除了",c"   ",d"   ",x"外，还有一些其他的参数可以加，见后面的附表。

　　然后我们看看变量a。a是个指针。看左边窗口，即使点了它的+号展开，也只看到了它指向的地址的第一个元素(49)。如果想要看得更多的数据，也可以像我一样，在上面的Memory窗口里看。但是Memory窗口只有一个，要看多个指针指向的数据就麻烦了，切来切去。那就在watch窗口中显示吧，a,4就可以了，看到我的watch窗口的显示没？所以，有了第二个小技巧：

2. watch窗口中，把指针当成数组看，只要在指针名后面加上一个长度，就可以想看数组一样看到对应的数据了。比如我上面的a,4。那么如果一个指针指向的数据很大，比如一个整形指针a是指向一个1000个整数的大块内存，我只想看看最后4个数据，要怎样呢？那就(a+996),4 呗。从第996个数据开始，看4个~

　　接下来这个小技巧是我最喜欢的一个了。调试中遇到系统函数调用失败的情况，通常都要加上GetLastError()函数获得返回值，然后查对应的解释才知道错误原因。比如，我上面的代码要打开一个不存在的文件，结果fopen失败。取回了错误码dwError=2，一查才知道是文件不存在。那么可不可以不用查，调试器直接告诉我原因呢？当然可以，不然我写这干嘛！刚才的错误码2是记录在dwError中了，所以只要在watch窗口看看dwError,hr看value栏：系统找不到指定的文件！爽吧！总结一下，第三个小技巧：

3. 在watch窗口中察看错误原因，只需要在错误之后面颊上",hr"就可以了。比如我上面的 dwError,hr 和 2, hr 都能够显示错误消息。想看某个错误码的解释，只要后面加上",hr"就轻松搞定，非常方便！这里还要提一下的是，即使不调用GetLastError()也是可以看错误原因的。在fopen()调用完后，直接在watch窗口敲err,hr也可以显示最近一次的错误原因。但是我机器重新装了os，还没装vc，现在用的还是安装前的尸体。所以这个err,hr的显示有问题。不过还是有应对之法，那就是强大的TIB信息。watch窗口看看*(unsigned long*)(TIB+0x34), hr也是一样的。至于为什么吗，那就是GetLastError的机制了。

　　还剩2个变量，ae和ce。这两变量类型名不同，但是其他全都一样。为什么这两个变量在watch窗口中显示的不一样呢？ae直接显示出了类成员的值，ce就显示了个......嗯，这就是最后一个小技巧：

4. 在vc安装目录的msdev\bin目录下有个autoexp.dat文件。可以在里面自定义数据的显示。具体的看看它前面的大段说明，说得很详细。我就是在文件后面加上一行AutoExpand =int_val=<a,d> sz_val=<p,s>。

## 附表：

### 下表说明调试器可识别的格式说明符。

|说明符|格式|值|显示|
|:-:|:-|:-|:-|
|d,i|	signed 十进制整数|	0xF000F065|	-268373915|
|u	|unsigned 十进制整数	|0x0065	|101
|o|	unsigned 八进制整数	|0xF065	|0170145|
|x,X	|十六进制整数	|61541（十进制）	|0x0000F065
|l,h	|用于 d、i、u、o、x、X 的 long 或 short 前缀	|00406042,hx|	0x0c22
|f	|signed 浮点型	|3./2.|	1.500000|
|e	|signed 科学计数法	|3./2.	|1.500000e+000
|g	|signed 浮点型或 signed |科学计数法，无论哪个都更短	|3./2.|	1.5
|c	|单个字符	|0x0065	|101 |'e'|
|s	|字符串	|0x0012fde8	|"Hello world"|
|su	|Unicode 字符串	 	"Hello world"
|hr	|HRESULT 或 Win32 错误代码。|（调试器自动将 HRESULT 解码，因此这些情况下不需要该说明符。	|0x00000000L|	S_OK
|wc	|窗口类标志。	|0x00000040	|WC_DEFAULTCHAR|
|wm	|Windows 消息数字	|0x0010	|WM_CLOSE|


### 下表包含用于内存位置的格式化符号。

|符号|格式|显示|
|:-:|:-|:-|
|ma	|64 个 ASCII 字符	|0x0012ffac .4...0...".0W&.......1W&.0.:W..1...."..1.JO&.1.2.."..1...0y....1
|m	|以十六进制表示的 16 个字节，后跟 16 个 ASCII 字符	|0x0012ffac B3 34 CB 00 84 30 94 80 FF 22 8A 30 57 26 00 00 .4...0...".0W&..
|mb	|以十六进制表示的 16 个字节，后跟 16 个 ASCII 字符	|0x0012ffac B3 34 CB 00 84 30 94 80 FF 22 8A 30 57 26 00 00 .4...0...".0W&..
|mw	|8 个字	0x0012ffac |34B3 00CB 3084 8094 22FF 308A 2657 0000
|md	|4 个双倍字长	|0x0012ffac 00CB34B3 80943084 308A22FF 00002657
|mq	|2 个四倍字长	|0x0012ffac 7ffdf00000000000 5f441a790012fdd4
|mu	|2 字节字符 (Unicode)|	0x0012fc60 8478 77f4 ffff ffff 0000 0000 0000 0000
　　可以使用带有计算为位置的任何值或表达式的内存位置说明符。

