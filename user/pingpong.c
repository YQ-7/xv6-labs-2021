#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{

    int pid;
    int fds[2];
    char buf[100];

    // 创建一个pipe,文件描述符由参数filedes数组返回
    // fd[0]：管道的读取段
    // fd[1]：管道的写入段
    pipe(fds);

    pid = fork();
    if (0 == pid)
    {
        read(fds[0], buf, sizeof(buf));
        printf("%d: received %s\n", getpid(), buf);
        write(fds[1], "pong", 4);
    }
    else
    {
        write(fds[1], "ping", 4);
        wait((int *)0);
        read(fds[0], buf, sizeof(buf));
        printf("%d: received %s\n", getpid(), buf);
    }

    exit(0);
}