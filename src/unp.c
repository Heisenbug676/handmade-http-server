#include "unp.h"

// err_sys:报错函数 fmt:自定义信息 errno:全局变量 perror函数会自动帮你报错且翻译全局变量报错的信息
void err_sys(const char *fmt)
{
    perror(fmt);
    exit(1);
}

// Socket函数：是个socket的包裹函数
int Socket(int family, int type, int protocol)
{
    int n;
    if ((n = socket(family, type, protocol)) < 0)
    {
        err_sys("socket函数调用失败");
    }
    return n;
}
// Bind函数：将listenfd（监听套接字）绑定到对应的服务器端口和IP地址上
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
    {
        err_sys("bind error!");
    }
}
// Listen函数：将listen函数激活，进入监听状态
void Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
    {
        err_sys("listen error!");
    }
}
// Accept函数：接受客户端发来的请求并进入阻塞状态
int Accept(int fd, struct sockaddr *clia, socklen_t *clialen)
{
    int n;
again:
    if ((n = accept(fd, clia, clialen)) < 0)
    {
        if (errno == EINTR)
            goto again;
        else
            err_sys("accept error");
    }
    return n;
}
// Connect函数：客户端发起TCP三次握手 这里的se实际是个指针 在实际函数调用中用&的方式等价转换成指针传到此处来
void Connect(int fd, const struct sockaddr *se, socklen_t selen)
{
    if (connect(fd, se, selen) < 0)
    {
        err_sys("connect error");
    }
}
// Fork函数：accept函数返回一个connfd时fork一个子循环进行下一步操作
pid_t Fork(void)
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        err_sys("fork error");
    }
    return pid;
}
// 处理粘包问题(tcp的字节流）：readline和writen函数
ssize_t writen(int fd, const void *vptr, size_t n) // 发到哪去 发的内容 发的长度
{
    ssize_t nwritten; // 单次写入的数量
    size_t nleft;     // 剩余的
    const char *ptr;  // char类型的指针一次走1个字节

    nleft = n;
    ptr = vptr; // 指针指在起始位置
    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft); // ptr：从哪里开始写 nleft：写多少（一般是全放进去）
        if (nwritten <= 0)                // if特殊情况处理
        {
            if (nwritten == -1 && errno == EINTR) //==判断运算符和=赋值运算符区分下能否
            {
                nwritten = 0;
            }
            else
            {
                return -1;
            }
        }

        ptr += nwritten;   // 正常调用write后指针需要移到正确位置 方便下次继续
        nleft -= nwritten; // 需要确保输入了多少，防止从头开始
    }
    return n;
}
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    char *ptr, c; // c是暂存读取的数据 放在*ptr（存储仓库）是归宿
    ssize_t rc, n;
    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
    again:
        rc = read(fd, &c, 1);
        if (rc == 1) // rc（返回代码）为1：正确读取到字符了（包括换行 因此加了if语句）
        {
            *ptr = c;
            ptr++;
            if (c == '\n')
            {
                break;
            }
        }

        else if (rc == 0) // rc为0：对方关闭连接 封上口 防止乱码
        {
            *ptr = '\0';
            return n - 1;
        }
        else
        {
            if (errno == EINTR) // 这种情况下属于正常信号中断 无碍again重新read
            {
                goto again;
            }
            return -1; // 以上都不符合就报错
        }
    }
    *ptr = '\0'; // 依旧封口
    return n;
}
// signal函数：处理信号
Sigfunc *Signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact; // 定义结构体
    act.sa_handler = func;
    sigemptyset(&act.sa_mask); // 重置信号集sa_mask      // 填充act
    act.sa_flags = 0;
    if (signo == SIGALRM) // if else
    {
#ifdef SA_INTERRUPT
        act.sa_flags = SA_INTERRUPT;
#endif // 所有预处理指令都需要独占一行
    }
    else
    {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
    if (sigaction(signo, &act, &oact) < 0) // 调用signation(signo,&act,&ocat)
        return (SIG_ERR);                  // 失误了返回
    return (oact.sa_handler);              // 成功了返回oact给其备用
}

// sig_child函数：子进程死掉后，父进程进行的一系列操作，防止出现僵尸进程
void sig_child(int signo) // 一般是sigchild信号
{
    int stat;                                       // 子进程死掉的状态
    pid_t pid;                                      // 死掉子进程的id
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) // WNOHANG别在这死等 有死去的子进程就处理 没有返回0就不要等了
    {
        printf("子进程%d dead", pid);
    }
    return;
}
// Close函数
void Close(int fd)
{
    if (close(fd) == -1)
    {
        err_sys("close error");
    }
}
// handle_login函数：在accept接收后 进行听浏览器发过来的信息 在终端打印出信息 回复连接成功的信息
void handle_login(int sockfd)
{
    char buf[MAXLINE];
    int n;
    int content_len = 0; // 记录body内容多大

    while ((n = readline(sockfd, buf, MAXLINE)) > 0)//先读head
    {
        printf("%s", buf);
        if (strncasecmp(buf, "Content-Length:", 15) == 0)
        {
            content_len = atoi(&buf[15]);
        }
        if (strcmp("\r\n", buf) == 0 || strcmp("\n", buf) == 0)
        {
            break;
        }
    }
    if (content_len > 0)//再读body
    {
        int read_n = read(sockfd, buf, content_len);
        if (read_n > 0)
        {
            buf[read_n] = '\0';
        }
        printf("终端显示：%s", buf);
    }
    // 最后回信 sprintf函数作用是把发送的内容按照http格式填到response里
    char *my_reply = "登录成功！";
    char response[MAXLINE];
    sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type:text/plain;charset=utf-8\r\n"
            "Access-Control-Allow-Origin:*\r\n"
            "Content-Length:%ld\r\n"
            "\r\n"
            "%s",
            strlen(my_reply), my_reply);
    // 发送出去
    writen(sockfd, response, strlen(response));
}