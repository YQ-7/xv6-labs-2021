#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LEN 512

int main(int argc, char *argv[])
{
    if (argc - 1 >= MAXARG)
    {
        fprintf(2, "xargs: too many arguments.\n");
        exit(1);
    }

    char *x_argv[MAXARG];   //exec的参数数组
    char line_buf[MAX_LEN]; // 存储读取的一行字符
    char c;                 // 每次读取的字符
    int cc;                 // 每次读取字符，read返回值

    // 存储初始参数
    int i;
    for (i = 1; i < argc; i++)
    {
        x_argv[i - 1] = argv[i];
    }

    int line_begin = 0;       //定位参数起始位置
    int line_end = 0;         //定位参数结束始位置
    int arg_init_cnt = i - 1; // 初始参数次序
    do
    {
        line_begin = 0;
        line_end = 0;
        char *p = line_buf;
        int arg_cnt = arg_init_cnt; // 当前参数次序

        // 读取一行参数
        while (1)
        {
            // 读取标准输入的一个字符
            cc = read(0, &c, 1);
            if (cc > 0)
            {
                if (line_end + 1 >= MAX_LEN)
                {
                    fprintf(2, "xargs: too many input words.\n");
                    exit(1);
                }
                line_buf[line_end++] = c;
            }

            // 将完整的单词设置为参数
            if (cc < 1 || c == ' ' || c == '\n')
            {
                x_argv[arg_cnt++] = &line_buf[line_begin];
                *p = 0;
                line_begin = line_end; // 下一个参数次序
            }

            if (cc < 1 || c == '\n')
            {
                break;
            }

            ++p;
        }

        if (cc < 1 && c == '\n')
        {
            break;
        }

        // fork子进程
        if (fork() == 0)
        {
            // 子进程执行命令
            exec(argv[1], x_argv);
            printf("exec failed!\n");
            exit(1);
        }
        else
        {
            // 父进程等待子进程执行完成
            wait(0);
        }

        // 重置参数
        memset(x_argv[arg_init_cnt], 0, arg_cnt);
        memset(line_buf, 0, MAX_LEN);

    } while (cc > 0);
    exit(0);
}
