#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ_PORT 0
#define WRITE_PORT 1
#define MAX_PRIME 35

void child(int fd_read);

void child(int fd_read) {
    int p[2];
    pipe(p);  // 创建新的管道

    int prime;
    if (read(fd_read, &prime, sizeof(int)) == 0) {
        // 如果没有更多的数字，退出子进程
        exit(0);
    }
    printf("prime: %d\n", prime);

    if (fork() == 0) {
        // 子进程递归
        close(p[WRITE_PORT]);  // 关闭写端
        child(p[READ_PORT]);    // 继续递归
        close(p[READ_PORT]);    // 关闭读端
        exit(0);
    } else {
        // 父进程处理
        close(p[READ_PORT]);    // 关闭读端
        int n;
        while (read(fd_read, &n, sizeof(int)) != 0) {
            if (n % prime != 0) {
                write(p[WRITE_PORT], &n, sizeof(int));  // 写入不被当前质数整除的数
            }
        }
        close(p[WRITE_PORT]);   // 关闭写端
        close(fd_read);         // 关闭父进程的读端
        wait(0);  // 需要先close再wait，不然进程会阻塞 -v
        exit(0);
    }
}

int main(int argc, char const *argv[]) {
    int p[2];
    pipe(p);  // 创建父进程的管道

    if (fork() == 0) {
        // 子进程：递归处理
        close(p[WRITE_PORT]);  // 关闭写端
        child(p[READ_PORT]);    // 调用递归函数
        close(p[READ_PORT]);    // 关闭读端
        exit(0);
    } else {
        // 父进程：喂数据
        close(p[READ_PORT]);  // 关闭读端
        for (int i = 2; i <= MAX_PRIME; i++) {
            write(p[WRITE_PORT], &i, sizeof(int));  // 向管道中写入数据
        }
        close(p[WRITE_PORT]);  // 写完数据后关闭写端
        wait(0);  // 等待子进程
        exit(0);
    }

    exit(0);
}