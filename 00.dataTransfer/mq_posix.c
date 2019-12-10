#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_BUF_SIZE        8192
#define POSIX_MQ_NAME       "/MessageQueuePOSIX"

// struct mq_attr{
//     long mq_flags;      /* Flags (ignored for mq_open()     */
//     long mq_maxmsg;     /* Max. # of messages on queue      */
//     long mq_msgsize;    /* Max. message size (bytes)        */
//     long mq_curmsgs;    /* # of message currently in queue  */
// };                      /* (ignored for mq_open())          */

static void
print_usage(const char *idxname)
{
    printf("%s error. index : (snd[1] prio[2] msg[3] | rcv[1]).\n", idxname);
}

static mqd_t
createMsgQ(void)
{
    int rtn;
    mqd_t mqd;
    struct mq_attr attr;

    //printf("[CREATE SPACE] PID : %d \n", getpid());

    mqd = mq_open(POSIX_MQ_NAME, O_RDWR | O_CREAT, 0644, NULL);
    if (mqd == -1) {
        perror("mq_open() \n");
        return -1;
    }

    memset(&attr, 0x00, sizeof(attr));

    rtn = mq_getattr(mqd, &attr);
    if (rtn == -1) {
        perror("mq_getattr() \n");
        rtn = mq_close(mqd);
        if (rtn == -1) {
            perror("close() \n");
            return -1;
        }
        return -1;
    }

#if 1
    printf("mq_flags   = %ld \n", attr.mq_flags);
    printf("mq_maxmsg  = %ld \n", attr.mq_maxmsg);
    printf("mq_msgsize = %ld \n", attr.mq_msgsize);
    printf("mq_curmsgs = %ld \n", attr.mq_curmsgs);
#endif

    return mqd; 
}

static int
doSndQue(unsigned int prio, const char *msg)
{
    int rtn;
    mqd_t mqd;

    mqd = createMsgQ();

    //printf("[SENDQ SPACE] PID : %d \n", getpid());

    rtn = mq_send(mqd, msg, strlen(msg), prio);
    if (rtn == -1) {
        perror("mq_send() \n");
        rtn = mq_close(mqd);
        if (rtn == -1) {
            perror("close() \n");
            return -1;
        }
        return -1;    
    }
    
    return 0;
}

static int
doRcvQue(void)
{
    ssize_t rtn;
    unsigned int priority;
    char rbuf[MAX_BUF_SIZE];
    mqd_t mqd;

    mqd = createMsgQ();

    //printf("[RECEIVEQ SPACE] PID : %d \n", getpid());

    rtn = mq_receive(mqd, rbuf, sizeof(rbuf), &priority);
    if (rtn == -1) {
        perror("mq_receive() \n");
        rtn = mq_close(mqd);
        if (rtn == -1) {
            perror("close() \n");
            return -1;
        }
        return -1;    
    }

    printf("PRIO : %u, MSG : %s, PID : %d \n",priority, rbuf, getpid());

    return 0;
}

int 
main(int argc, char const *argv[])
{
    int rtn;
    unsigned int priority;
    char sbuf[MAX_BUF_SIZE];

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    //printf("[COMMON SPACE] PID : %d \n", getpid());

    if (!strcmp(argv[1], "snd")) {
        /* Message Send */
        if (argc < 4) {
            print_usage(argv[2]);
            print_usage(argv[3]);
            return -1;
        }

        priority = (unsigned int)strtol(argv[2], NULL, 10);
        strcpy(sbuf, argv[3]);

        rtn = doSndQue(priority, sbuf);
        if (rtn == -1) {
            perror("doSndQue() \n");
            return -1;
        }

    } else if (!strcmp(argv[1], "rcv")) {
        /* Message Receive */
        rtn = doRcvQue();
        if (rtn == -1) {
            perror("doRcvQue() \n");
            return -1;
        }
    } else {
        /* Error Case */
        print_usage(argv[1]);
        return -1;
    }

    return 0;
}
