/*
    1. 读取标准输入
    2. 根据空格和'\n'划分参数
    3. 调用fork+exec
 */
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAX_LEN 50

int main(int argc, char const *argv[])
{
    // argv : xargs cmd cmd_arg cmd_arg...
    if(argc < 2) {
        fprintf(2, "usage: xargs cmd argv\n");
        exit(1);
    }

    char* cmd = argv[1];
    char buf;
    char params[MAXARG][MAX_LEN];
    int i;
    for(i = 1; i < argc; i++) {
        // printf("argv: %s\n", argv[i]);
        // 取出了xargs的所有参数，包括cmd，因为exec的argv[0]是cmd
        strcpy(params[i-1], argv[i]); 
    }

    int res;
    int count = argc-1;
    int cursor = 0;
    while((res = read(0, &buf, 1)) > 0) {
        if(buf == '\n') {
            if(fork() == 0) {
                char* param[MAXARG];
                for(int i = 0; i <= count; i++){
                    param[i] = params[i];
                    // printf("param: %s, count = %d, cursor = %d\n", params[i], count, cursor);
                }
                
                param[count+1] = 0;
                exec(cmd, param);

                fprintf(2, "exec error\n");
                exit(1);
            }
            // 要在父进程重置，否则没用
            count = argc - 1;
            cursor = 0;
            memset(params, 0, MAXARG * MAX_LEN);
            wait(0);
        }else if(buf == ' ') {
            count++;
        }else{
            if(cursor == MAX_LEN-1) {
                fprintf(2, "xargs: MAX_LEN limit\n");
                exit(1);
            }
            params[count][cursor++] = buf;
        }
    }
    exit(0);
}
