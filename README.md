# Muduo 网络库

## 学习目的

> - **理解：阻塞 / 非阻塞、同步 / 异步**
> - **理解：Linux 上的五种 IO 模型【 ... 】**
> - **掌握 epoll 的原理和优势**
> - 认识并理解 Reactor 模型
> - 学习开源项目核心代码的设计思路
> - **掌握基于事件驱动和事件回调的 epoll + 线程池面向对象编程**
> - 深刻理解 muduo 网络库源码，以及使用 C++11 重构



----



## 知识储备

> - 传输层协议：TCP 协议 和 UDP 协议
> - 理解并掌握 TCP / UDP 编程步骤
> - 熟悉 Linux 上的 IO 多路复用
> - Linux 多线程编程



----



## 阻塞 / 非阻塞、同步 / 异步

### 基本认识

> **这四个关键词实际就是 IO 事件中与数据打交道时的不同处理方式。**
>
> ---
>
> **一次 IO 操作可以分成两个阶段**
>
> - **数据准备阶段**【根据系统IO操作的就绪状态】：阻塞 / 非阻塞
>   - **阻塞的表现**：recv 函数不返回！所处线程执行阻塞 “停止” 在 recv 函数处！
>     - 此时，读到数据的返回值是：所读取到的数据量大小！
>     - 阻塞的唤醒时机，有数据！
>   - **非阻塞的表现**：recv 函数会返回！所处线程正常执行！
>     - 如果 size = -1，有两层意义：
>       - 可能是非阻塞返回表现【errno = EAGAIN 或 errno = EWORLDBLOCK】
>       - 可能是系统内部错误
>     - size = 0 表示：网络对端关闭了连接！
> - **数据读写阶段**【根据应用程序和内核的交互方式】：同步 / 异步
>   - **同步**：数据从 TCP 接收缓冲区拷贝到 用户层的 buf 中，该过程的是耗时的，耗时在应用程序，而不是内核！
>     - 注意：recv 函数是同步接口！
>   - **异步**：就是数据的拷贝托管给操作系统，而用户层应用程序可以随心所以，如果操作系统完成了拷贝，就会 `通知` 应用程序，通知方式：信号 / 回调函数！
>
> ---
>
> 注意：并发时也有同步异步概念！
>
> **在处理 IO 的时候，阻塞和非阻塞都是同步 IO。只有使用了特殊的 API 才是异步IO。**

~~~c++
// IO 接口
ssize_t recv(int sockfd, void* buf, size_t len, int flags);
=> sockfd：数据源（文件描述符），是否阻塞就是看此处的文件描述符读取限制（阻塞/非阻塞）
=> buf：用户（应用程序）层的数据缓存
=> len：buf 的大小
=> 一般填：0

// 对于 TCP 
// 内核层有 TCP 接收缓冲区（就是我们读取的数据源）
1. 调用 recv 时，就是如果 TCP 接收缓冲区有数据，就开始把其中的数据 往用户层的 buf 里面搬【该过程中如果没有搬完，就不能直接向下执行，这个和是否阻塞无关，在计算机的角度，这个搬运速度是相当快的！时间消耗在应用程序层】
2. recv 函数返回后，就是说明搬运完成，buf 中有数据了！
~~~



![](.\pic\QQ截图20240206170551.png)





### 理解性总结

> **阻塞与非阻塞**：
>
> - 关键点在于调用了 IO 操作接口后，当数据未就绪时，是否会阻塞当前线程。也就是这个时候当前线程能否继续执行其他任务！
>
> ---
>
> **同步和异步**：
>
> - **同步**：就是 A 向 B 提出想要进行某种操作，B 直接提供必要的资源，其他操作 A 自行完成！A 无法做 其他事！
>   - 操作是 提出者 自己完成！
> - **异步**：就是 A 把想做的操作和操作完成后的反馈（或者说通知 A 操作完成的方式）告诉 B，也就是甩手掌柜式的将任务托付给 B，A 只要知道是否完成，同时 A 可以做其他事！
>   - 操作托管给其他人！





### 总结说明

> **阻塞：指调用结果返回之前，当前线程会被挂起。调用线程只有在得到结果之后才会返回。**
>
> **非阻塞：指在不能立刻得到结果之前，该调用不会阻塞当前线程。线程无需等待，该返回带有特定的错误码（EAGAIN / EWORLDBLOCK）**
>
> - 非阻塞 IO 操作需要程序员主动设置反复尝试读取文件描述符，以尝试读取数据！该操作叫做：【轮询】！
> - 对于 CPU 来说，这是性能上的较大消耗，一般只有特定情形才用！
>
> 阻塞与非阻塞关注的是：程序在 【`进行 IO 操作时`】 是否会被阻塞，即是否等待 IO 操作完成 / 程序在处理 I/O 操作时是否需要等待。
>
> **是否阻塞的根本因素在于：被读取的文件描述符属性！（阻塞 / 非阻塞）**
>
> ---
>
> **同步 IO：同步 I/O 操作是一种阻塞式的操作方式。在同步 I/O 中，程序在发起一个 I/O 操作后会一直等待该操作完成，然后才能继续执行后续的代码。**这意味着程序会在进行 I/O 操作期间被阻塞，无法执行其他任务，直到 I/O 操作完成。
>
> **异步 IO：异步 I/O 操作是一种非阻塞式的操作方式。在异步 I/O 中，程序发起一个 I/O 操作后，不需要等待该操作完成，而是可以继续执行后续的代码。**程序会注册一个回调函数（Callback），该函数将在 I/O 操作完成时被调用，通知程序数据已经就绪。
>
> - 使用异步 I/O 的操作方式，程序可以在等待 I/O 操作完成的同时继续执行其他任务，从而**提高了程序的并发性和性能。**
> - 异步操作通常需要一些额外的编程机制来处理回调函数的调用和数据就绪的通知。
>
> 同步与异步关注的是：程序在 【`发起IO操作后`】 是否需要等待IO操作完成，即是否等待IO操作的结果。



### 面试时的回答方式

> 阻塞 / 非阻塞、同步 / 异步 是用来描述 IO 操作的状态的！
>
> 一个典型的 IO 操作，应该包含：数据准备 和 数据读写！



---



## Linux 五种 IO 模型

> 五种模型：
>
> - 阻塞 IO、非阻塞 IO、信号驱动 IO 、IO 多路复用、异步 IO



### 阻塞 IO【Blocking】

> **阻塞IO：在内核将数据准备好之前，系统调用会一直等待。**
>
> - 所有的套接字（ bind、accept、recv、send、listen、recvfrom、... ），默认都是阻塞方式。

![](.\pic\QQ截图20240206192635.png)



### 非阻塞 IO【Non-Blocking】

> **非阻塞 IO：如果内核还未将数据准备好，系统调用仍然会直接返回，并且返回 EWOULDBLOCK 错误码！**
>
> - **非阻塞 IO 往往需要程序员循环的方式反复尝试读写文件描述符，这个过程称为【轮询】**。**这对CPU来说是较大的浪费, 一般只有特定场景下才使用。**
>
> ---
>
> 设置方式：

~~~c++
setblockopt(sockfd);
~~~





![](.\pic\QQ截图20240206192646.png)



### 信号驱动 IO【Signal-Driven】

> **信号驱动 IO：内核将数据准备好的时候，使用 SIGIO 信号通知应用程序进行IO操作。**
>
> ---
>
> 逻辑（使用信号驱动 IO）：
>
> - 应用的进程注册好信号处理程序
> - 该程序会递交给内核，该程序就是包含了资源“请求”信息。告知操作系统去准备或检查资源，准备好了就通知进程，这个时间段内进程可以继续执行！
> - 进程收到信号后，就说明了数据资源就绪！
> - 进程可以去读取数据了，但是依旧会调用一个默认为阻塞的接口去读取数据。
>
> ---
>
> **内核在第一个阶段是异步，在第二个阶段是同步**；
>
> **与非阻塞IO的区别在于它提供了消息通知机制**，不需要用户进程不断的轮询检查，减少了系统API的调用次数，提高了效率。

![](.\pic\QQ截图20240206192708.png)



### IO 多路复用【IO Multiplexing】

> **IO 多路转接：虽然从流程图上看起来和阻塞 IO 类似。实际上最核心在于 IO 多路转接能够同时等待多个文件描述符的就绪状态。**
>
> - select 等函数含有多个文件描述符！该函数相当于在等和拷贝中只负责等！recvfrom 只负责拷贝数据（此时无需等待，读取时一定是有数据的！）
> - 有设置是否阻塞以及轮询周期的参数！

![](.\pic\QQ截图20240206192657.png)



### 异步 IO 【Asynchronous】

> **异步 IO：由内核在数据拷贝完成时，通知应用程序（而信号驱动是告诉应用程序何时可以开始拷贝数据）**

![](.\pic\QQ截图20240206192726.png)



~~~c++
// 异步 IO 使用到的结构体
struct aiocb {
	int aio_fildes,
	off_t aio_offset,
	volatile void *aio_buf,
	size_t aio_nbytes,
	int aio_reqprio,
	struct sigevent aio_sigevent,
	int aio_lio_opcode
};
~~~





---



## 良好的网络服务器设计思路

### 设计思路来源

> 在这个多核处理器的时代，服务端网络编程如何选择线程模型呢？ 
>
> - 关于多核的优势：可以同时执行多个指令或任务。 每个核心都可以独立地执行指令和处理任务。 多核处理器可以将负载分配到不同的核心上，从而实现并行处理。
>
> ---
>
> libev 作者的观点：one loop per thread is usually a good model. 【每个线程一个循环，通常是一个很好的模型。】
>
> 基于这一点，我们可以比较容易的反应到：对于一个多线程服务端的设计，就可以转换成设计多个事件循环（event loop），只要让每个线程去处理一个事件循环，同时如果事件循环具备高效且易于使用，不就是一个比较良好的服务端吗！



### 关于数据与事件循环的杂谈

> 就我个人浅薄的认识而言，我们处于计算机时代，网络时代，我们与世界的沟通方式无非就是数据！
>
> 我们通过图片看到的世界，就是计算机或其他设备完成了特定格式数据的格式化，我们才看见了多姿多彩的世界！
>
> 我们和世界的交流就是通过数据！
>
> 我不否认存在其他方法的交流，比如面对面交流等！
>
> ---
>
> 那么这里要谈论的 `事件循环` 和数据之间有什么关系？
>
> 我的解释是：事件循环 “操作” 的对象就是数据（每个人想法或认识不同，这是我的观点！）



### 事件循环的讨论

> 先来**结论**：
>
> - **事件循环（event loop）是 non-blocking 网络编程的核心！**
>
> 再来说 “常规设计思路”：
>
> - **非阻塞 IO 和 IO 多路复用 几乎总是一起使用！**
>
> ---
>
> **讨论：**
>
> **为什么用非阻塞 IO、而不是 阻塞 IO？**
>
> - 显然，它们的最大区别在于：**执行 IO 操作并的到结果前，两者所处线程的运行状态！**
> - 阻塞 IO，会挂起线程，而无法继续执行后续任务！对于一个服务器而言，阻塞式的操作是致命的。
> - 非阻塞 IO，可以不用一直等待系统调用反馈结果，系统操盘就绪数据时，可以同时执行其他任务！
>
> **非阻塞 IO 的缺点是什么？**（对于性能要求高的场景）
>
> - **缺点在于：需要主动【`轮询`】检查数据就绪状态 / IO 操作完成情况！浪费 CPU 资源！**
> - 对于高性能服务器而言，无疑是不可取的！
>
> **IO 多路复用的优点是什么？**
>
> - **优点：能够同时监测多个文件描述符的状态变化。**
> - **同时，IO 多路复用的系统调用是支持阻塞和非阻塞的！**
> - **它有通知机制！该机制可以弥补非阻塞 IO 中的主动轮询操作！**





## Reactor 模型（事件驱动模型）

### 模型认识

> The reactor design pattern is an event handling pattern for handling service requests delivered concurrently to a service handler by one or more inputs. The service handler then demultiplexes the incoming requests and dispatches them synchronously to the associated request handlers.
>
> 反应器设计模式是一种事件处理模式，用于处理通过一个或多个输入并发地传递给服务处理程序的服务请求。然后，服务处理程序将传入的请求解复用，并将它们同步地分派给相关的请求处理程序。



### 模型组件

> - Event 事件
> - Reactor 反应堆
> - Demultiplex 事件分发器【 基于 IO 多路复用 】
> - Evanthandler 事件处理器
>
> ---



### 模型组件交互认识

> **Event 事件 => Reactor 反应堆：**（从事件到反应堆）
>
> - 首先，我们会注册事件和给定事件的处理（或者说是回调）方法！
>   - 该事件是我们要关注的东西，比如：TCP 接收缓冲区是否有数据【Eevnt】？
>   - 如果有？则进行读取【Handler】！
>
> ---
>
> **Reactor 反应堆**
>
> - 实际就是：事件和对应事件的处理方法的集合！
> - 如果要自己定义该集合的管理方式，会用什么数据结构？
>   - 一般用 map 表存储即可，键值映射。
> - 工作：Reactor 反应堆中的事件需要被监控 / 管理，对应的需要事件状态。
>   - epoll add：将待监控事件加入到 epoll 监控模型中。
>   - epoll mod：修改 epoll 监控模型中的事件
>   - epoll del：在 epoll 监控模型中删除某个事件
>
> ---
>
> **Reactor 反应堆 => Demultiplex 事件分发器**【一般就是用 epoll 】
>
> - **启动反应堆 => 开启事件循环 epoll_wait 监视事件的状态变化！**
> - 如果是服务器！
>   - 启动之后，就开始检测：新用户的连接请求事件，已连接用户的读写请求事件等。
> - 如果检测到事件变化，即某种事件就绪！就会给 Reactor 反应堆 进行事件返回！
>   - 为什么返回？事件分发器只是监测了事件就绪的准备情况，并不参与事件就绪的方法调用！
>
> ---
>
> **Reactor 反应堆 => Evanthandler 事件处理器：**
>
> - 就绪的事件，去调用相应的回调方法处理事件！
> - 如果是服务器：可能涉及数据的序列化与反序列化【 json、protobuf 等 】，数据包装/编码发送等等操作对客户端用户操作进行响应。



![](.\pic\QQ截图20240206211516.png)





### Muduo Reactor 模型

> 

![](.\pic\QQ截图20240206214022.png)



---



## Epoll IO 多路复用初步认识



### 三种多路复用的选择（为什么是 Epoll）

#### select 和 poll 的缺点

> - **存在可监视的文件描述符的最大限制！**数量可以修改！
>   - **关于 select ：select 采用轮询的方式进行扫描文件描述符，文件描述符越多，性能越差！**
> - **内核 / 用户空间内存拷贝问题**
>   - select 需要复制大量的句柄数据结构，产生巨大的开销
> - **select 返回的是含有整个句柄的数组**
> - **select 的触发方式是水平触发**
>
> **关于 select 的缺点：**
>
> - **存在可监视的文件描述符数量限制！**可以修改！
>   - 但是：**select 采用轮询的方式进行扫描文件描述符，文件描述符越多，性能越差！**
> - **内核 与 用户空间内存拷贝问题**
>   - select 需要复制大量的句柄数据结构，产生巨大的开销
> - **select 返回的是含有整个句柄的数组**，应用程序需要**遍历整个数组**才能发现哪些句柄发生了事件。
> - **select 的触发方式是水平触发**，应用程序如果没有完成对一个已经就绪的文件描述符进行IO操作，那么之后每次select调用还是会将这些文件描述符通知进程。
>
> ---
>
> **关于 poll ：**
>
> - **相比 select 模型，poll 使用链表保存文件描述符，没有了监视文件数量的限制，但是其他缺点依旧存在！**
>
> ---
>
> 以 select 模型为例，假设我们的服务器需要支持100万的并发连接，则在 __FD_SETSIZE 为1024的情况下，则我们至少需要开辟1k个进程才能实现100 万的并发连接。除了进程间上下文切换的时间消耗外，从内核/用户空间大量的句柄结构内存拷贝、数组轮询等，是系统难以承受的。因此，基于 select 模型的服务器程序，要达到 100 万级别的并发访问，是一个很难完成的任务。
>
> ---
>
> **试想：如果使用服务器进程每次都把这100万个连接告诉操作系统，使用 select / poll 就需要把句柄数据从用户态复制句柄数据结构到内核态，等操作系统完成了对事件变化的检查后，需要在再次句柄数据复制到用户态，最后用户层需要通过遍历的方式轮询句柄处理已发生的网络事件。**
>
> **总体下来，资源消耗巨大！**
>
> 因此，select/poll一般只能处理几千的并发连接。



### Epoll 原理及其优势

> epoll 的实现机制与 select / poll 机制完全不同，它们的缺点在 epoll 上不复存在。
>
> ---
>
> epoll 的涉及设计实现方式：
>
> - epoll 通过在 Linux 内核中申请一个简易文件系统，把 select / poll 中调用分成三个部分！
>   - epoll_create()：创建 epoll 对象
>     - 在epoll文件系统中为这个句柄对象分配资源
>   - epoll_ctl()：向对象中添加待监视事件
>   - epoll_wait()：收集发生的事件的文件描述符资源
> - 总体逻辑上：只需要在进程启动时建立一个 epoll 对象，然后在需要的时候向这个 epoll 对象中添加或者删除事件。同时，epoll_wait 的效率也非常高，因为调用 epoll_wait 时，并没有向操作系统复制这 100 万个连接的句柄数据，内核也不需要去遍历全部的连接。
>
> eventpoll 结构体：

~~~c++
struct eventpoll{
	....
	/*红黑树的根节点，这颗树中存储着所有添加到epoll中的需要监控的事件*/
	struct rb_root rbr;
	/*双链表中则存放着将要通过epoll_wait返回给用户的满足条件的事件*/
	struct list_head rdlist;
	....
};
~~~







### 两种工作模式（LT / ET）

#### 水平触发【LT】

> 如果内核数据就绪！且没有被读完，就会一直触发通知机制。



#### 边沿触发【ET】

> 如果内核数据就绪！只对用户通知一次，无论是否读取完！



### muduo 采用的是 LT 及事件处理优点

> - **不会丢失数据或者消息**
>   - 应用没有读取完数据，内核是会不断上报的
> - **低延迟处理**
>   - 每次读数据只需要一次系统调用；照顾了多个连接的公平性，不会因为某个连接上的数据量过大而影响其他连接处理消息
> - **跨平台处理**
>   - 像 select 一样可以跨平台使用



---



## Epoll 的原理理解

### 原理概括

> **epoll 底层实现中有两个关键的数据结构，一个是 eventpoll 另一个是 epitem**
>
> - **eventpoll 中有两个成员变量分别是 rbr 和 rdlist，前者指向一颗红黑树的根，后者指向双向链表的头。** 
> - **epitem 则是红黑树节点和双向链表节点的综合体，也就是说 epitem 即可作为树的节点，又可以作为链表的节点，并且 epitem 中包含着用户注册的事件。**
>
> - 当用户调用 epoll_create() 时，会创建 eventpoll 对象（包含一个红黑树和一个双链表）；
> - 而用户调用 epoll_ctl(ADD) 时，会在红黑树上增加节点（epitem 对象）；
> - 接下来，操作系统会默默地在通过 epoll_event_callback() 来管理 eventpoll 对象。当有事件被触发时，操作系统则会调用 epoll_event_callback 函数，将含有该事件的 epitem 添加到双向链表中。
> - 当用户需要管理连接时，只需通过 epoll_wait() 从 eventpoll 对象中的双链表下"摘取" epitem 并取出其包含的事件即可。



### epoll 中的相关方法或结构

#### 结构

##### epoll_event 事件信息结构体

~~~c++
struct epoll_event {
    // 用户的关注需求 / OS 反馈动作
    uint32_t events;      /* Epoll events */	
    // 关注的对象（文件描述符）
    epoll_data_t data;    /* User data variable */	
};

typedef union epoll_data {	// 联合体：使用其中任意一个即可
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;
~~~



##### 事件类型的宏定义

~~~c++
【重点】=> EPOLLIN : 表示对应的文件描述符可以读 (包括对端SOCKET正常关闭）。 

【重点】=> EPOLLOUT : 表示对应的文件描述符可以写。

EPOLLPRI : 表示对应的文件描述符有紧急的数据可读 (这里应该表示有带外数据到来）。
EPOLLERR : 表示对应的文件描述符发生错误。
EPOLLHUP : 表示对应的文件描述符被挂断。
EPOLLET : 将EPOLL设为边缘触发（Edge Triggered）模式, 这是相对于水平触发（Level Triggered）来说的。

【重点】=> EPOLLONESHOT：只监听一次事件, 当监听完这次事件之后, 如果还需要继续监听这个socket的话, 需要再次把这个socket加入到EPOLL队列里。
~~~



##### epitem 结构体

> **epitem 是中包含了两个主要的成员变量，分别是 rbn 和 rdlink，前者是红黑树的节点，而后者是双链表的节点**，也就是说一个 epitem 对象即可作为红黑树中的一个节点又可作为双链表中的一个节点。并且每个 epitem 中存放着一个 event，对 event 的查询也就转换成了对 epitem 的查询。

~~~c++
struct epitem {
	RB_ENTRY(epitem) rbn;
	/*  RB_ENTRY(epitem) rbn等价于
	struct {											
		struct type *rbe_left;		//指向左子树
		struct type *rbe_right;		//指向右子树
		struct type *rbe_parent;	//指向父节点
		int rbe_color;			    //该节点的颜色
	} rbn
	*/
 
	LIST_ENTRY(epitem) rdlink;
	/* LIST_ENTRY(epitem) rdlink等价于
	struct {									
		struct type *le_next;	//指向下个元素
		struct type **le_prev;	//前一个元素的地址
	}*/
 
	int rdy; //判断该节点是否同时存在与红黑树和双向链表中
	
	int sockfd; //socket句柄
	struct epoll_event event;  //存放用户填充的事件
};
~~~





##### eventpoll 结构体

> **eventpoll 中包含了两个主要的成员变量，分别是 rbr 和 rdlist，前者指向红黑树的根节点，后者指向双链表的头结点。**
>
> 即一个 eventpoll 对象对应二个 epitem 的容器。对 epitem 的检索，将发生在这两个容器上（红黑树和双向链表）。

~~~c++
struct eventpoll{ 
	.... 
	/* 红黑树的根节点，这颗树中存储着所有添加到epoll中的需要监控的事件 */ 
	struct rb_root rbr; 		
	/* 双链表中则存放着将要通过epoll_wait返回给用户的满足条件的事件 */ 
	struct list_head rdlist; 	
	.... 
};
// =====================================================================
struct eventpoll {
	/*
	struct ep_rb_tree {
		struct epitem *rbh_root; 			
	}
	*/
	ep_rb_tree rbr;      //rbr指向红黑树的根节点
	
	int rbcnt; //红黑树中节点的数量（也就是添加了多少个TCP连接事件）
	
	LIST_HEAD( ,epitem) rdlist;    //rdlist指向双向链表的头节点；
	/*	这个LIST_HEAD等价于 
		struct {
			struct epitem *lh_first;
		}rdlist;
	*/
	
	int rdnum; //双向链表中节点的数量（也就是有多少个TCP连接来事件了）
 
	// ...略...
	
};
~~~









#### 三个系统调用 + 一个自动调用

##### epoll_create

> **作用：创建一个 epoll 句柄**
>
> **注意点：用完之后必须调用 close() 关闭**
>
> ---
>
> **底层操作：**
>
> - **构建红黑树**【管理所有注册的事件，epitem 结构体对象（一个通用结点结构体，可以记录 epoll_event 事件信息结构体）】
> - **创建回调方法**【就是事件就绪后，操作系统通知用户层的方法】
> - **构建就绪队列**【即：双向链表：管理所有就绪的事件】
>
> ---
>
> **返回值： eventpoll 句柄 id【文件描述符】**

~~~c++
int epoll_create(int size);

// 工作内容：
=> 创建 eventpoll 对象
=> 让 eventpoll 中的 rbr 指向空
=> 让 eventpoll 中的 rdlist 指向空
=> 在并发环境下进行互斥
=> 保存 eventpoll 对象
=> 返回 eventpoll 对象的句柄（id）

// ====================================================================
// 源码：简览
//创建epoll对象，包含一颗空红黑树和一个空双向链表
int epoll_create(int size) {
	//与很多内核版本一样，size参数没有作用，只要保证大于0即可
	if (size <= 0) return -1;
	
	nty_tcp_manager *tcp = nty_get_tcp_manager(); //获取tcp对象
	if (!tcp) return -1;
	
	struct _nty_socket *epsocket = nty_socket_allocate(NTY_TCP_SOCK_EPOLL);
	if (epsocket == NULL) {
		nty_trace_epoll("malloc failed\n");
		return -1;
	}
 
	// 1° 开辟了一块内存用于填充eventpoll对象
	struct eventpoll *ep = (struct eventpoll*)calloc(1, sizeof(struct eventpoll));
	if (!ep) {
		nty_free_socket(epsocket->id, 0);
		return -1;
	}
 
	ep->rbcnt = 0;
 
	// 2° 让红黑树根指向空
	RB_INIT(&ep->rbr);       //等价于ep->rbr.rbh_root = NULL;
 
	// 3° 让双向链表的头指向空
	LIST_INIT(&ep->rdlist);  //等价于ep->rdlist.lh_first = NULL;
 
	// 4° 并发环境下进行互斥
	// ...该部分代码与主线逻辑无关，可自行查看...
 
	//5° 保存epoll对象
	tcp->ep = (void*)ep;
	epsocket->ep = (void*)ep;
 
	return epsocket->id;
}
~~~









##### epoll_ctl

> - **epoll 的事件注册函数**
> - 返回值：成功：0
>
> ---
>
> 将用户传入的参数封装为一个 epitem 对象，然后根据传入的 op 是：
>
> - EPOLL_CTL_ADD：将 epitem 对象插入红黑树
> - EPOLL_CTL_MOD：更新红黑树中的 epitem 对象
> - EPOLL_CTL_DEL：移除红黑树中的 epitem 对象

~~~c++
// 使用方式：
// 1. 建立 epoll_event 结构体
// 2. 使用 epoll_ctl 系统调用，把带监视的事件加入到红黑树中
int epoll_ctl(int epid, int op, int sockid, struct epoll_event *event);
// 参数解释
=> int epid：eventpoll 句柄
=> int op：控制操作：增加、修改、删除
=> int sockid：被操作的文件描述符！
=> struct epoll_event *event：准备好的 epoll_event【包含事件操作动作（读/写/...）和关注的文件描述符】

// ====================================================================
// 源码：简览
// 往红黑树中加每个tcp连接以及相关的事件
int epoll_ctl(int epid, int op, int sockid, struct epoll_event *event) {
 
	nty_tcp_manager *tcp = nty_get_tcp_manager();
	if (!tcp) return -1;
 
	nty_trace_epoll(" epoll_ctl --> 1111111:%d, sockid:%d\n", epid, sockid);
	struct _nty_socket *epsocket = tcp->fdtable->sockfds[epid];
 
	if (epsocket->socktype == NTY_TCP_SOCK_UNUSED) {
		errno = -EBADF;
		return -1;
	}
 
	if (epsocket->socktype != NTY_TCP_SOCK_EPOLL) {
		errno = -EINVAL;
		return -1;
	}
 
	nty_trace_epoll(" epoll_ctl --> eventpoll\n");
 
	struct eventpoll *ep = (struct eventpoll*)epsocket->ep;
	if (!ep || (!event && op != EPOLL_CTL_DEL)) {
		errno = -EINVAL;
		return -1;
	}
 
	if (op == EPOLL_CTL_ADD) {
		//添加sockfd上关联的事件
		pthread_mutex_lock(&ep->mtx);
 
		struct epitem tmp;
		tmp.sockfd = sockid;
		struct epitem *epi = RB_FIND(_epoll_rb_socket, &ep->rbr, &tmp); //先在红黑树上找，根据key来找，也就是这个sockid，找的速度会非常快
		if (epi) {
			//原来有这个节点，不能再次插入
			nty_trace_epoll("rbtree is exist\n");
			pthread_mutex_unlock(&ep->mtx);
			return -1;
		}
 
		//只有红黑树上没有该节点【没有用过EPOLL_CTL_ADD的tcp连接才能走到这里】；
 
		//(1)生成了一个epitem对象，这个结构对象，其实就是红黑的一个节点；
		epi = (struct epitem*)calloc(1, sizeof(struct epitem));
		if (!epi) {
			pthread_mutex_unlock(&ep->mtx);
			errno = -ENOMEM;
			return -1;
		}
		
		//(2)把socket(TCP连接)保存到节点中；
		epi->sockfd = sockid;  //作为红黑树节点的key，保存在红黑树中
 
		//(3)我们要增加的事件也保存到节点中；
		memcpy(&epi->event, event, sizeof(struct epoll_event));
 
		//(4)把这个节点插入到红黑树中去
		epi = RB_INSERT(_epoll_rb_socket, &ep->rbr, epi); //实际上这个时候epi的rbn成员就会发挥作用，如果这个红黑树中有多个节点，那么RB_INSERT就会epi->rbi相应的值：可以参考图来理解
		assert(epi == NULL);
		ep->rbcnt ++;
		
		pthread_mutex_unlock(&ep->mtx);
 
	} else if (op == EPOLL_CTL_DEL) {
		pthread_mutex_lock(&ep->mtx);
 
		struct epitem tmp;
		tmp.sockfd = sockid;
		
		struct epitem *epi = RB_FIND(_epoll_rb_socket, &ep->rbr, &tmp);//先在红黑树上找，根据key来找，也就是这个sockid，找的速度会非常快
		if (!epi) {
			nty_trace_epoll("rbtree no exist\n");
			pthread_mutex_unlock(&ep->mtx);
			return -1;
		}
		
		//只有在红黑树上找到该节点【用过EPOLL_CTL_ADD的tcp连接才能走到这里】；
 
		//从红黑树上把这个节点移除
		epi = RB_REMOVE(_epoll_rb_socket, &ep->rbr, epi);
		if (!epi) {
			nty_trace_epoll("rbtree is no exist\n");
			pthread_mutex_unlock(&ep->mtx);
			return -1;
		}
 
		ep->rbcnt --;
		free(epi);
		
		pthread_mutex_unlock(&ep->mtx);
 
	} else if (op == EPOLL_CTL_MOD) {
		struct epitem tmp;
		tmp.sockfd = sockid;
		struct epitem *epi = RB_FIND(_epoll_rb_socket, &ep->rbr, &tmp); //先在红黑树上找，根据key来找，也就是这个sockid，找的速度会非常快
		if (epi) {
			//红黑树上有该节点，则修改对应的事件
			epi->event.events = event->events;
			epi->event.events |= EPOLLERR | EPOLLHUP;
		} else {
			errno = -ENOENT;
			return -1;
		}
 
	} else {
		nty_trace_epoll("op is no exist\n");
		assert(0);
	}
 
	return 0;
}
~~~





##### epoll_wait



> 方法作用：到双向链表中去取相关的事件通知
>
> 具体：先看一下 eventpoll 对象的双链表中是否有节点。如果有节点的话则取出节点中的事件填充到用户传入的指针所指向的内存中。如果没有节点的话，则在while循环中等待一定时间，直到有事件被触发后操作系统会将epitem插入到双向链表上使得rdnum>0时(这个过程是由操作系统调用epoll_event_callback函数完成的)，程序才会跳出while循环，去双向链表中取数据。

~~~c++
int epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout);
// 参数解释
=> int epid：eventpoll 句柄
=> struct epoll_event *events：获取到摘取的 epoll_event 信息
=> int maxevents：单次可获取就绪事件个数的最大值
=> int timeout：函数调用方式：0：非阻塞等待！-1：阻塞式等待！>0：定时主动反馈！


// ====================================================================
// 源码：简览
//到双向链表中去取相关的事件通知
int epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout) {
 
	nty_tcp_manager *tcp = nty_get_tcp_manager();
	if (!tcp) return -1;
 
	struct _nty_socket *epsocket = tcp->fdtable->sockfds[epid];
 
	struct eventpoll *ep = (struct eventpoll*)epsocket->ep;
	
    // ...此处主要是一些负责验证性工作的代码...
 
	//(1)当eventpoll对象的双向链表为空时，程序会在这个while中等待一定时间，
	//直到有事件被触发，操作系统将epitem插入到双向链表上使得rdnum>0时，程序才会跳出while循环
	while (ep->rdnum == 0 && timeout != 0) {
		// ...此处主要是一些与等待时间相关的代码...
	}
 
 
	pthread_spin_lock(&ep->lock);
 
	int cnt = 0;
 
	//(1)取得事件的数量
	//ep->rdnum：代表双向链表里边的节点数量（也就是有多少个TCP连接来事件了）
	//maxevents：此次调用最多可以收集到maxevents个已经就绪【已经准备好】的读写事件
	int num = (ep->rdnum > maxevents ? maxevents : ep->rdnum); //哪个数量少，就取得少的数字作为要取的事件数量
	int i = 0;
	
	while (num != 0 && !LIST_EMPTY(&ep->rdlist)) { //EPOLLET
 
		//(2)每次都从双向链表头取得 一个一个的节点
		struct epitem *epi = LIST_FIRST(&ep->rdlist);
 
		//(3)把这个节点从双向链表中删除【但这并不影响这个节点依旧在红黑树中】
		LIST_REMOVE(epi, rdlink); 
 
		//(4)这是个标记，标记这个节点【这个节点本身是已经在红黑树中】已经不在双向链表中；
		epi->rdy = 0;  //当这个节点被操作系统 加入到 双向链表中时，这个标记会设置为1。
 
		//(5)把事件标记信息拷贝出来；拷贝到提供的events参数中
		memcpy(&events[i++], &epi->event, sizeof(struct epoll_event));
		
		num --;
		cnt ++;       //拷贝 出来的 双向链表 中节点数目累加
		ep->rdnum --; //双向链表里边的节点数量减1
	}
	
	pthread_spin_unlock(&ep->lock);
 
	//(5)返回 实际 发生事件的 tcp连接的数目；
	return cnt; 
}
~~~





##### epoll_event_callback()

> **作用：向双向链表中（就绪队列中）增加就绪结点（就绪事件信息）。**
>
> - 即：把红黑树中所监视的结点中的就绪结点转移到双向链表中。
>
> ---
>
> 方法被调用的情形：
>
> - 客户端 connect() 连入，服务器处于SYN_RCVD状态时
> - 三路握手完成，服务器处于 ESTABLISHED 状态时
> - 客户端 close() 断开连接，服务器处于 FIN_WAIT_1 和 FIN_WAIT_2 状态时
> - 客户端 send/write() 数据，服务器可读时
> - 服务器可以发送数据时

~~~c++
//当发生客户端三路握手连入、可读、可写、客户端断开等情况时，操作系统会调用这个函数，用以往双向链表中增加一个节点【该节点同时 也在红黑树中】
int epoll_event_callback(struct eventpoll *ep, int sockid, uint32_t event) {
	struct epitem tmp;
	tmp.sockfd = sockid;
 
	//(1)根据给定的key【这个TCP连接的socket】从红黑树中找到这个节点
	struct epitem *epi = RB_FIND(_epoll_rb_socket, &ep->rbr, &tmp);
	if (!epi) {
		nty_trace_epoll("rbtree not exist\n");
		assert(0);
	}
 
	//(2)从红黑树中找到这个节点后，判断这个节点是否已经被连入到双向链表里【判断的是rdy标志】
	if (epi->rdy) {
		//这个节点已经在双向链表里，那无非是把新发生的事件标志增加到现有的事件标志中
		epi->event.events |= event;
		return 1;
	} 
 
	//走到这里，表示 双向链表中并没有这个节点，那要做的就是把这个节点连入到双向链表中
 
	nty_trace_epoll("epoll_event_callback --> %d\n", epi->sockfd);
	
	pthread_spin_lock(&ep->lock);
 
	//(3)标记这个节点已经被放入双向链表中，我们刚才研究epoll_wait()的时候，从双向链表中把这个节点取走的时候，这个标志被设置回了0
	epi->rdy = 1;  
 
	//(4)把这个节点链入到双向链表的表头位置
	LIST_INSERT_HEAD(&ep->rdlist, epi, rdlink);
 
	//(5)双向链表中的节点数量加1，刚才研究epoll_wait()的时候，从双向链表中把这个节点取走的时候，这个数量减了1
	ep->rdnum ++;
 
	pthread_spin_unlock(&ep->lock);
	pthread_mutex_lock(&ep->cdmtx);
	pthread_cond_signal(&ep->cond);
	pthread_mutex_unlock(&ep->cdmtx);
 
	return 0;
}
~~~









---













