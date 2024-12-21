#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ_PORT 0
#define WRITE_PORT 1
#define BUFFER_SIZE 1

int
main(int argc, char* argv[])
{
    int p_2_partent[2]; // 父进程接收的管道
    int p_2_child[2];   // 子进程接收的管道
    
    // 创建两个管道
    pipe(p_2_partent);
    pipe(p_2_child);
    
    if (fork() == 0) {
        // 子进程
        close(p_2_partent[READ_PORT]);  // 关闭父进程读取端
        close(p_2_child[WRITE_PORT]);   // 关闭父进程写入端
        
        // ping：子进程阻塞读
        char buffer[BUFFER_SIZE];
        read(p_2_child[READ_PORT], buffer, BUFFER_SIZE);  // 读取父进程的ping消息
        printf("%d: received ping\n", getpid());
        close(p_2_child[READ_PORT]);  // 子进程读取完毕后关闭读端
        
        // pong：子进程发送
        const char* message = "pong";
        write(p_2_partent[WRITE_PORT], message, strlen(message) + 1);  // 发送pong消息
        close(p_2_partent[WRITE_PORT]);  // 发送完毕后关闭写端
        exit(0);
    } else {
        // 父进程
        close(p_2_partent[WRITE_PORT]);  // 关闭父进程写入端
        close(p_2_child[READ_PORT]);     // 关闭父进程读取端
        
        // ping：父进程发送
        const char* message = "ping";
        write(p_2_child[WRITE_PORT], message, strlen(message) + 1);  // 发送ping消息
        close(p_2_child[WRITE_PORT]);  // 发送完毕后关闭写端
        
        // pong：父进程阻塞读
        wait(0);
        char buffer[BUFFER_SIZE];
        read(p_2_partent[READ_PORT], buffer, BUFFER_SIZE);  // 读取子进程的pong消息
        printf("%d: received pong\n", getpid());
        close(p_2_partent[READ_PORT]);  // 读取完毕后关闭读端
        exit(0);
    }

    exit(0);
}