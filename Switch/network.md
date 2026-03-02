#### 主机序和网络序

小端字节序被称为主机字节序，网络数据流应采用大端字节序。

0x12345678，大端的话从低地址到高地址为0x12,0x34,0x56,0x78.也就是低地址高位，高地址地位。小端则相反。

```c
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```



#### sockaddr_in结构体

```c
struct sockaddr_in {
    sa_family_t sin_family; /* Address family */ 地址结构类型
    u_int16_t sin_port; /* Port number */ 端口号
    struct in_addr sin_addr; /* Internet address */ IP 地址
    /* Pad to size of `struct sockaddr'. */
    unsigned char __pad[__SOCK_SIZE__ - sizeof(short int) -
    sizeof(unsigned short int) - sizeof(struct in_addr)];//unsigned char[8]
};

struct in_addr { /* Internet address. */
    u_int32_t s_addr;
};

struct sockaddr_in6 {
    sa_family_t sin6_family; /* AF_INET6 */
    u_int16_t sin6_port; /* Transport layer port # */
    u_int32_t sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr; /* IPv6 address */
    u_int32_t sin6_scope_id; /* scope id (new in RFC2553) */
};

struct in6_addr {
    unsigned char sa_addr[16];
};
```



#### IP地址转换

IP地址例如192.168.1.1需要转换成整数才能使用

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
in_addr_t inet_addr(const char *cp);//将点分十进制IP字符串转换为网络字节序整数表示的IPv4地址
int inet_aton(const char *cp, struct in_addr *inp); //和inet_addr一样，只是转换后的数据保存在inp
char *inet_ntoa(struct in_addr in);//将网络字节序整数表示的IPv4地址转换为点分十进制的字符串表示。


//上面的只适合ipv4，下面的ipv4和ipv6都适合
#include <arpa/inet.h>
int inet_pton(int af, const char *src, void *dst);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```



#### socket函数

```c
#include <sys/types.h> 
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```

  

| 参数名   | 作用                        | 常用取值                                                     | 说明                                                         |
| -------- | --------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| domain   | 地址族（通信范围 / 底层协议 | AF_INET（IPv4）     AF_INET6（IPv6）     AF_UNIX（本地 IPC）     AF_PACKET（链路层）     AF_NETLINK（内核 - 用户态） | 决定地址格式（如 IP + 端口 / MAC 地址 /  本地路径）          |
| type     | 套接字类型（传输特性）      | SOCK_STREAM（TCP）     SOCK_DGRAM（UDP）     SOCK_RAW（原始套接字）     SOCK_SEQPACKET（有序数据包） | 决定数据传输的核心特性（可靠 / 不可靠、面向连接 / 无连接）   |
| protocol | 具体协议                    | 0（默认）     IPPROTO_TCP（TCP）     IPPROTO_UDP（UDP）     ETH_P_ALL（链路层所有帧） | 通常传 0，由系统根据 domain+type  自动选择最优协议；特殊场景需指定具体协议 |

| 组合示例                                       | 用途                 | 典型场景                                            |
| ---------------------------------------------- | -------------------- | --------------------------------------------------- |
| socket(AF_INET,  SOCK_STREAM, 0)               | 创建 IPv4 TCP 套接字 | 所有基于 TCP 的通信（HTTP/SSH/ 咱们写的 TCP  示例） |
| socket(AF_INET,  SOCK_DGRAM, 0)                | 创建 IPv4 UDP 套接字 | 实时通信（语音 / 视频）、SNMP 监控                  |
| socket(AF_UNIX,  SOCK_STREAM, 0)               | 创建本地域流式套接字 | 单机进程间通信（比 TCP 回环快）                     |
| socket(AF_PACKET,  SOCK_RAW, htons(ETH_P_ALL)) | 创建链路层原始套接字 | OMCI 报文收发、网络抓包                             |
| socket(AF_NETLINK,  SOCK_RAW, NETLINK_ROUTE)   | 创建 Netlink 套接字  | 内核 - 用户态通信（路由 / 网卡状态通知）            |



#### bind 函数

服务于需要bind端口号和地址后来进行监听客户端的连接，客户端可以不需要bind

```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

bzero(&servaddr, sizeof(servaddr));
```



#### listen函数

```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int listen(int sockfd, int backlog);
//listen()的backlog应该指定某个给定套接字上内核为之排队的最大已完成连接数。
```



#### accept 函数

服务端使用accept函数

```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
//返回值：成功返回一个新的socket 文件描述符，用于和客户端通信，失败返回-1，设置errno
```



#### connect 函数

客户端使用connet函数。connet里面的addr参数需要和服务端bind的参数保存一致。

```c
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/*常见的两种错误是ECONNREFUSED和ETIMEOUT。
ECONNREFUSED：目标端口不存在，连接被拒绝
ETIMEOUT：连接超时
*/
```



#### close 函数

```c
#include <unistd.h>
int close(int fd);
```



#### shutdown 函数

```c
#include <sys/socket.h>
int shutdown(int sockfd, int how);
/*
how: 允许为shutdown 操作选择以下几种方式:
SHUT_RD(0)： 关闭sockfd 上的读功能，此选项将不允许sockfd 进行读操作 该套接字不再接受数据，任何当前在套接字接受缓冲区的数据将被无声的丢弃掉
SHUT_WR(1): 关闭sockfd 的写功能，此选项将不允许sockfd 进行写操作。进程不能在对此套接字发出写操作
SHUT_RDWR(2): 关闭sockfd 的读写功能。相当于调用shutdown 两次：首先是SHUT_RD,然后SHUT_WR
*/
```



#### getsockname和getpeername函数

```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *localaddr, &addrlen);
int getpeername(int sockfd, struct sockaddr *peeraddr, &addrlen); // 返回：成功为0， 出错为-1
```



#### sockatmark--带外标记

该函数用于判断sockfd是否处于带外标记，即下一个被读到的数据是否是带外数据。如果是则返回1，这时就可以用带MSG_OOB标志的recv调用来接收带外数据。如果不是则返回0.

```c
#include <sys/socket.h>
int sockatmark(int sockfd);
```



#### getsockopt和setsockopt函数

```c
#include <sys/types.h>
#include <sys/socket.h>
int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen);
```

SOL_SOCKET(通用层)----服务端和/客户端都要会

| 选项名              | 作用                                        | 代码示例（核心片段）                                         |
| ------------------- | ------------------------------------------- | ------------------------------------------------------------ |
| SO_REUSEADDR        | 允许复用端口（解决重启报 “Address in use”） | int opt=1; setsockopt(fd,  SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));（bind  前设） |
| SO_RCVTIMEO         | 设置 recv/accept 超时（防永久阻塞）         | struct timeval t={5,0};  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t)); |
| SO_SNDTIMEO         | 设置 send/connect 超时                      | 同上，optname 换为SO_SNDTIMEO                                |
| SO_KEEPALIVE        | 开启 TCP 保活（检测对端断连）               | int opt=1; setsockopt(fd,  SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)); |
| SO_SNDBUF/SO_RCVBUF | 设置收发缓冲区大小                          | int buf=65536; setsockopt(fd,  SOL_SOCKET, SO_RCVBUF, &buf, sizeof(buf)); |

IPPROTO_TCP(TCP专属层)----低延迟/长连接必用

| 选项名       | 作用                                   | 代码示例                                                     |
| ------------ | -------------------------------------- | ------------------------------------------------------------ |
| TCP_NODELAY  | 关闭 Nagle 算法（降低小数据包延迟）    | int opt=1; setsockopt(fd,  IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)); |
| TCP_KEEPIDLE | TCP 保活空闲时间（多久没数据开始检测） | int idle=30; setsockopt(fd,  IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)); |



#### recv和send函数

`send()`/`recv()` 不保证 “一次性收发 len 字节”，原因：

- `send()`：内核发送缓冲区可能满，只能发送部分数据；
- `recv()`：对端可能分批次发送，内核接收缓冲区只有部分数据。

阻塞 vs 非阻塞模式

- 默认阻塞模式：

  - `recv()`：无数据时阻塞，直到有数据 / 连接关闭 / 超时；
  - `send()`：发送缓冲区满时阻塞，直到有空闲空间。

- 非阻塞模式

  （需通过 fcntl 设置 O_NONBLOCK）：

  - 无数据 / 缓冲区满时立即返回 `-1`，`errno = EAGAIN/EWOULDBLOCK`；
  - 适合高并发场景（如 `epoll` 事件驱动）。

```c
#include <sys/socket.h>
ssize recv(int sockfd, void *buff, size_t nbytes, int flags);
ssize send(int sockfd, void *buff, size_t nbytes, int flags); // 返回：成功为读入或写入的字节数，出错为-1
```



#### sendto 和 recvfrom 函数

`sendto()` 和 `recvfrom()` 函数 —— 这是 **UDP 通信的核心函数**（也兼容 TCP），解决了 `send()`/`recv()` 无法指定对端地址的问题，是无连接通信的必备工具

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
```



#### socketpair 函数

实际上socketpair 函数跟pipe 函数是类似的，也只能在同个主机上具有亲缘关系的进程间通信，但pipe 创建的匿名管道是半双工的，而socketpair 可以认为是创建一个全双工的管道。

```c
#include <sys/types.h>
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int sv[2]);
//sv: 返回套接字对
```



#### recvmsg 和 sendmsg 函数

```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
ssizt_t sendmsg(int sockfd, struct msghdr *msg, int flags);

struct msghdr {
    void          *msg_name;            /* protocol address */
    socklen_t     msg_namelen;          /* sieze of protocol address */
    struct iovec  *msg_iov;             /* scatter/gather array */
    int           msg_iovlen;           /* # elements in msg_iov */
    void          *msg_control;         /* ancillary data ( cmsghdr struct) */
    socklen_t     msg_conntrollen;      /* length of ancillary data */
    int           msg_flags;            /* flags returned by recvmsg() */
}

#include <sys/uio.h>
struct iovec {
    void    *iov_base;      /* starting address of buffer */
    size_t  iov_len;        /* size of buffer */
}
```



#### dup 和 dup2 函数

有时希望将标准输入重定向到一个文件，或者把标准输出重定向到一个网络连接（例如CGI编程）。这里可以使用下面的用于复制文件描述符的dup 或者dup2 函数来实现：

```c
#include <unistd.h>
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```



#### readv 和 writev 函数

readv 函数将数据从文件描述符读到分散的内存块中，即分散读；writev 函数则将多块分散的内存数据一并写入文件描述符中，即集中写。数据是iovec 结构体。例如：

```c
struct iovec iv[2];
iv[ 0 ].iov_base = header_buf; // char header_buf[ BUFFER_SIZE ];
iv[ 0 ].iov_len = strlen( header_buf );
iv[ 1 ].iov_base = file_buf; // char* file_buf;
iv[ 1 ].iov_len = file_stat.st_size;
ret = writev( connfd, iv, 2 );
```

