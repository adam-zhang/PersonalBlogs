# the description for IPC in Linux

在linux下的多个进程间的通信机制叫做IPC(Inter-Process Communication)，它是多个进程之间相互沟通的一种方法。在linux下有多种进程间通信的方法：半双工管道、命名管道、消息队列、信号、信号量、共享内存、内存映射文件，套接字等等。使用这些机制可以为linux下的网络服务器开发提供灵活而又坚固的框架。

## 1. 管道 （PIPE）

管道实际是用于进程间通信的一段共享内存，创建管道的进程称为管道服务器，连接到一个管道的进程为管道客户机。一个进程在向管道写入数据后，另一进程就可以从管道的另一端将其读取出来。

管道的特点：

1. 管道是半双工的，数据只能向一个方向流动；需要双方通信时，需要建立起两个管道；
2. 只能用于父子进程或者兄弟进程之间（ 具有亲缘关系的进程）。 比如fork或exec创建的新进程， 在使用exec创建新进程时，需要将管道的文件描述符作为参数传递给exec创建的新进程。 当父进程与使用fork创建的子进程直接通信时，发送数据的进程关闭读端，接受数据的进程关闭写端。
3. 单独构成一种独立的文件系统：管道对于管道两端的进程而言，就是一个文件，但它不是普通的文件，它不属于某种文件系统，而是自立门户，单独构成一种文件系统，并且只存在与内存中。
4. 数据的读出和写入：一个进程向管道中写的内容被管道另一端的进程读出。写入的内容每次都添加在管道缓冲区的末尾，并且每次都是从缓冲区的头部读出数据。

管道的实现机制：

    管道是由内核管理的一个缓冲区，相当于我们放入内存中的一个纸条。管道的一端连接一个进程的输出。这个进程会向管道中放入信息。管道的另一端连接一个进程的输入，这个进程取出被放入管道的信息。一个缓冲区不需要很大，它被设计成为环形的数据结构，以便管道可以被循环利用。当管道中没有信息的话，从管道中读取的进程会等待，直到另一端的进程放入信息。当管道被放满信息的时候，尝试放入信息的进程会等待，直到另一端的进程取出信息。当两个进程都终结的时候，管道也自动消失。
管道只能在本地计算机中使用，而不可用于网络间的通信。 
pipe函数原型：
```
#include <unistd.h>
int pipe(int file_descriptors[2]);
```
example
```
int fd[2];
int result = pipe(fd);
```
通过使用底层的read和write调用来访问数据。 向 file_descriptor[1]写 数据，从 file_descriptor[0]中 读数据。写入与读取的顺序原则是 先进先出。 
管道读写规则
当没有数据可读时
    O_NONBLOCK disable：read调用阻塞，即进程暂停执行，一直等到有数据来到为止。
    O_NONBLOCK enable：read调用返回-1，errno值为EAGAIN。
当管道满的时候
    O_NONBLOCK disable： write调用阻塞，直到有进程读走数据
    O_NONBLOCK enable：调用返回-1，errno值为EAGAIN
如果所有管道写端对应的文件描述符被关闭，则read返回0
如果所有管道读端对应的文件描述符被关闭，则write操作会产生信号SIGPIPE
当要写入的数据量不大于PIPE_BUF（Posix.1要求PIPE_BUF至少 512字节）时，linux将保证写入的原子性。
当要写入的数据量大于PIPE_BUF时，linux将不再保证写入的原子性。

## 命名管道(FIFO)

命名管道是一种特殊类型的文件，它在系统中以文件形式存在。这样克服了管道的弊端，他可以 允许没有亲缘关系的进程间通信。 
创建管道的两个系统调用原型：
```
#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char* filename, mode_t mode);
```
example
```
mkfifo("/tmp/cmd_pipe", S_FIFO|0666);
```

具体操作方法只要创建了一个命名管道然后就可以使用open、read、write等系统调用来操作。创建可以手工创建或者程序中创建。 

管道和命名管道的区别：
> 对于命名管道FIFO来说，IO操作和普通管道IO操作基本一样，但是两者有一个主要的区别，在命名管道中，管道可以是事先已经创建好的，比如我们在命令行下执行
```
mkfifo myfifo
```
>就是创建一个命名通道，我们必须用open函数来显示地建立连接到管道的通道，而在管道中，管道已经在主进程里创建好了，然后在fork时直接复制相关数据或者是用exec创建的新进程时把管道的文件描述符当参数传递进去。

> 一般来说FIFO和PIPE一样总是处于阻塞状态。也就是说如果命名管道FIFO打开时设置了读权限，则读进程将一直阻塞，一直到其他进程打开该FIFO并向管道写入数据。这个阻塞动作反过来也是成立的。如果不希望命名管道操作的时候发生阻塞，可以在open的时候使用O_NONBLOCK标志，以关闭默认的阻塞操作。

## 3. 信号 （signal）
信号机制是unix系统中最为古老的进程之间的通信机制，用于一个或几个进程之间传递异步信号。信号可以有各种异步事件产生，比如键盘中断等。shell也可以使用信号将作业控制命令传递给它的子进程。
在此列出几个简单使用方法定义： 
```
#include <sys/types.h>
#include <signal.h>
void (*signal(int sig, void (*func)(int)))(int);
```
```
int ret = signal(SIGSTOP, sighandle);
```
由于signal不够健壮，推荐使用sigaction函数。 
```
int kill(pid_t pid, int sig);
int raise(int sig);
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
int pause(void);
```

## 4. 消息队列（Message queues）
消息队列是内核地址空间中的内部链表，通过linux内核在各个进程直接传递内容，消息顺序地发送到消息队列中，并以几种不同的方式从队列中获得，每个消息队列可以用IPC标识符唯一地进行识别。内核中的消息队列是通过IPC的标识符来区别，不同的消息队列直接是相互独立的。每个消息队列中的消息，又构成一个独立的链表。

消息队列克服了信号承载信息量少，管道只能承载无格式字符流。 
消息队列头文件： 
```
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
```
1. 消息缓冲区结构：
```
struct msgbuf
{
	long mtype;
	char mtext[1];
}
```
在结构中有两个成员，mtype为消息类型，用户可以给某个消息设定一个类型，可以在消息队列中正确地发送和接受自己的消息。mtext为消息数据，采用柔性数组，用户可以重新定义msgbuf结构。例如：
```
struct msgbuf
{
	long mtype;
	char mtext[1];
}
```
当然用户不可随意定义msgbuf结构，因为在linux中消息的大小是有限制的，在linux/msg.h中定义如下：
```
#define MSGMAX 8192
```
消息总的大小不能超过8192个字节，包括mtype成员（4个字节）。

2. msqid_ds内核数据结构。
```
struct msqid_ds
{
	struct ipc_perm msg_perm;
	time_t msg_stime;
	time_t msg_rtime;
	time_t msg_ctime;
	unsigned long _msg_cbuyes;
	...
}
```
Linux内核中，每个消息队列都维护一个结构体，此结构体保存着消息队列当前状态信息，该结构体在头文件linux/msg.h中定义。
3. ipc_perm内核数据结构
```
struct ipc_perm
{
	key_t key;
	uid_t uid;
	gid_t gid;
	......
}
```
结构体ipc_perm保存着消息队列的一些重要的信息，比如说消息队列关联的键值，消息队列的用户id组id等。它定义在头文件linux/ipc.h中。
常用函数：
系统建立IPC通讯 （消息队列、信号量和共享内存） 时必须指定一个ID值。通常情况下，该id值通过ftok函数得到。 
消息队列的本质
Linux的消息队列(queue)实质上是一个链表，它有消息队列标识符(queue ID)。 msgget创建一个新队列或打开一个存在的队列；msgsnd向队列末端添加一条新消息；msgrcv从队列中取消息， 取消息是不一定遵循先进先出的， 也可以按消息的类型字段取消息。

消息队列与命名管道的比较
消息队列跟命名管道有不少的相同之处，通过与命名管道一样，消息队列进行通信的进程可以是不相关的进程，同时它们都是通过发送和接收的方式来传递数据的。在命名管道中，发送数据用write，接收数据用read，则在消息队列中，发送数据用msgsnd，接收数据用msgrcv。而且它们对每个数据都有一个最大长度的限制。
与命名管道相比，消息队列的优势在于，1、消息队列也可以独立于发送和接收进程而存在，从而消除了在同步命名管道的打开和关闭时可能产生的困难。2、同时通过发送消息还可以避免命名管道的同步和阻塞问题，不需要由进程自己来提供同步方法。3、接收程序可以通过消息类型有选择地接收数据，而不是像命名管道中那样，只能默认地接收。
————————————————
版权声明：本文为CSDN博主「yexz」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/a987073381/article/details/52006729
