#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primeProc(int inputFd)
{

    // 先读取第一个数字
    int first;
    if (0 == read(inputFd, &first, sizeof(first)))
    {
        close(inputFd);
        exit(0);
    }
    printf("prime %d\n", first);

    int fds[2];
    pipe(fds);
    if (0 == fork())
    {
        close(fds[1]);
        // 子进程消费管道数据
        primeProc(fds[0]);
    }
    else
    {
        close(fds[0]);
        int num;
        int eof;
        do
        {
            eof = read(inputFd, &num, sizeof(num));
            // 将无法整除的数写入管道
            if (0 != (num % first))
            {
                write(fds[1], &num, sizeof(num));
            }
        } while (eof);
        close(inputFd);
        close(fds[1]);
        wait((int *)0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int fds[2];

    pipe(fds);
    if (0 == fork())
    {
        close(fds[1]);
        // 子进程消费管道数据
        primeProc(fds[0]);
    }
    else
    {
        close(fds[0]);
        // 向管道写入数据
        for (int i = 2; i <= 35; i++)
        {
            write(fds[1], &i, sizeof(i));
        }
        close(fds[1]);
        wait((int *)0);
    }
    exit(0);
}
