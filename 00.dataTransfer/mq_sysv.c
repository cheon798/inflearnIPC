#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define IPC_KEY_PATHNAME    "/proc"
#define IPC_KEY_PROJ_ID     'A'

#define MAX_BUF_SIZE        1024

typedef struct {
    long mtype;
    char string[MAX_BUF_SIZE];
} mbuf_t;

static void
print_usage(const char *idxname)
{
    printf("%s error. index : (snd[1] mtype[2] msg[3] | rcv[1] mtype[2]). \n", idxname);
}

static key_t
makeKey(void)
{
    key_t key;

    key = ftok(IPC_KEY_PATHNAME, IPC_KEY_PROJ_ID);
    if (key == -1) {
        perror("ftok() \n");
        return -1;
    }

    printf("key : %x \n", key);

    return key;
}

static int
createMsgQID(void)
{
    int msgq_id;

    msgq_id = msgget(makeKey(), IPC_CREAT | 0644);
    if (msgq_id == -1) {
        perror("msgget() \n");
        return -1;    
    }

    return msgq_id; 
}

static int
doSndQue(const long mtype, const char *msg)
{
    int rtn, qid;
    mbuf_t mbuf;

    memset(&mbuf, 0x00, sizeof(mbuf));    
    mbuf.mtype = mtype;
    strncpy(mbuf.string, msg, strlen(msg));

    qid = createMsgQID();

    rtn = msgsnd(qid, &mbuf, strlen(mbuf.string), 0);
    if (rtn == -1) {
        perror("msgsnd() \n");
        return -1;
    }
    
    return 0;
}

static int
doRcvQue(const long mtype)
{
    int qid;
    ssize_t rtn;
    mbuf_t mbuf;

    memset(&mbuf, 0x00, sizeof(mbuf));    

    qid = createMsgQID();

    rtn = msgrcv(qid, &mbuf, sizeof(mbuf.string), mtype, 0);
    if (rtn == -1) {
        perror("msgrcv() \n");
        return -1;
    }

    printf("Received Message.. mtype : %ld, message : %s \n",
            mbuf.mtype, mbuf.string);

    return 0;
}

int 
main(int argc, char const *argv[])
{
    int rtn;
    long mtype;
    char sbuf[MAX_BUF_SIZE];

    if (argc < 3) {
        print_usage(argv[0]);
        return -1;
    }

    mtype = strtol(argv[2], NULL, 10);

    if (!strcmp(argv[1], "snd")) {
        /* Message Send */
        if (argc < 4) {
            print_usage(argv[3]);
            return -1;
        }

        if (mtype <= 0) {
            print_usage(argv[2]);
            return -1;
        }
        strcpy(sbuf, argv[3]);
        rtn = doSndQue(mtype, sbuf);
        if (rtn == -1) {
            perror("doSndQue() \n");
            return -1;
        }
    } else if (!strcmp(argv[1], "rcv")) {
        /* Message Receive */
        rtn = doRcvQue(mtype);
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
