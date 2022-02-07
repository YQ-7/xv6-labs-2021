#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * @brief 
 * 
 * @param dirPath 目录名称
 * @param name 搜索的文件名称
 */
void find(char *dirPath, char *name)
{
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    // 读取目录内容
    if ((fd = open(dirPath, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", dirPath);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dirPath);
        close(fd);
        return;
    }
    if (st.type != T_DIR)
    {
        fprintf(2, "find: %s not dir\n", dirPath);
        close(fd);
        return;
    }

    //父目录路径
    if (strlen(dirPath) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "ls: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, dirPath);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // 拼接目录名称
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        // 读取文件状态
        if (stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch (st.type)
        {
        case T_FILE: //文件
        {
            // 判断文件名是否匹配
            if (strcmp(de.name, name) == 0)
            {
                printf("%s\n", buf);
            }
            break;
        }
        case T_DIR: //目录
        {
            // 跳过.和..
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            {
                break;
            }
            //递归查找
            find(buf, name);
            break;
        }
        default:
            break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: find path name\n");
        exit(2);
    }
    find(argv[1], argv[2]);
    exit(0);
}