#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int 
main(int argc, char **argv)
{
    int rtn, wstatus, pipefd[2];
    char rbuf[1024];
    char wbuf[1024];
    pid_t cpid;

    printf("\n");
    printf("[%d] start of function \n\n", getpid());

    memset(rbuf, 0, sizeof(rbuf));
    memset(wbuf, 0, sizeof(wbuf));

    rtn = pipe(pipefd);
    if (rtn != 0) {
        perror("pipe()");
        goto err;
    }

    cpid = fork();
    switch (cpid)
    {
        case -1:    /* error case */
            perror("fork()");
            goto err;
            break;
        
        case 0:     /* child process */
            printf("[child]  PID  : %d \n", getpid());
            printf("[child]  PPID : %d \n\n", getppid());
            close(pipefd[1]);
            read(pipefd[0], rbuf, sizeof(rbuf));
            printf("parent said : %s \n\n", rbuf);
            close(pipefd[0]);
            break;
        
        default:    /* parent process */
            printf("[parent] PID  : %d \n", getpid());
            printf("[parent] PPID : %d \n\n", getppid());
            close(pipefd[0]);
            strncpy(wbuf, "my son, I love you.", sizeof(wbuf)-1);
            write(pipefd[1], wbuf, sizeof(wbuf));
            close(pipefd[1]);

            cpid = wait(&wstatus);
            printf("[child] PID : %d is finished. \n\n", cpid);
            break;
    }

    err :
        return -1;
        close(pipefd[0]);
        close(pipefd[1]);

    return 0;
}