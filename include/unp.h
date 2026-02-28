/* 
 * 防止头文件被重复包含的“保护锁”
 * 逻辑：如果没定义过 __unp_h，就定义它并编译里面的内容；如果定义过了，直接跳过。
 */
#ifndef __unp_h
#define __unp_h

/* --- 1. 包含所有必要的系统头文件 --- */
/* 这些是 Linux 网络编程的“基本法”，缺少任何一个都会报 incomplete type 错误 */
#include <sys/types.h>    // 基本系统数据类型
#include <sys/socket.h>   // 基本套接字结构和函数（如 struct sockaddr）
#include <sys/wait.h>    // waitpid 函数及其宏定义
#include <netinet/in.h>   // 网际网协议族（如 struct sockaddr_in）
#include <arpa/inet.h>    // IP地址转换函数（如 inet_pton）
#include <errno.h>        // 错误号定义
#include <stdio.h>        // 标准输入输出
#include <stdlib.h>       // 标准库（exit等）
#include <string.h>       // 字符串处理
#include <unistd.h>       // Unix 标准定义（fork, close, read, write）
#include <signal.h>       // 信号处理相关（struct sigaction）

/* --- 2. 定义常用的常量 --- */
#define MAXLINE 4096      // 缓冲区大小，一页内存的大小
#define LISTENQ 1024      // Listen 函数的排队队列长度
#define SA struct sockaddr // UNP 经典的简化写法：用 SA 代替长长的结构体名

/* --- 3. 定义信号处理函数指针类型 --- */
/* 逻辑：Sigfunc 代表一个函数，它接收一个 int 参数，返回 void。
   这会让后面 signal 函数的声明看起来更简洁。 */
typedef void Sigfunc(int);

/* --- 4. 包裹函数声明：基础功能 --- */
void err_sys(const char *fmt);
int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
/* 注意：Accept 的第三个参数必须是指针，因为它是“值-结果”参数 */
int Accept(int fd, struct sockaddr *clia, socklen_t *clialen);
void Connect(int fd, const struct sockaddr *se, socklen_t selen);
pid_t Fork(void);
void Close(int fd);

/* --- 5. 包裹函数声明：健壮的 I/O --- */
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);

/* --- 6. 包裹函数声明：信号处理 --- */
Sigfunc *Signal(int signo, Sigfunc *func);
void sig_child(int signo);

/* --- 7. 业务逻辑函数 --- */
/* 逻辑：专门处理浏览器登录请求的函数 */
void handle_login(int sockfd);

#endif /* __unp_h */