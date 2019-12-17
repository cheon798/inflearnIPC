#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF_SIZE    1024
#define RW_BUF_SIZE     3
#define SOCK_PATH       "sockStreamFile"

static void printUsage(const char *idxname);
static int tcpServ(void);
static int tcpClie(const char *request);

int 
main(int argc, char const *argv[])
{
    int rtn;
    char rqbuf[MAX_BUF_SIZE];

    memset(rqbuf, 0x00 , sizeof(rqbuf));

    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server")) {
        /* server */
        rtn = tcpServ();
        if (rtn == -1) {
            perror("tcpServ() \n");
            return -1;
        }

    } else if (!strcmp(argv[1], "client")) {
        /* client */
        if (argc <3) {
            printUsage(argv[2]);
            return -1;
        }

        strcpy(rqbuf, argv[2]);

        rtn = tcpClie(rqbuf);
        if (rtn == -1) {
            perror("tcpClie() \n");
            return -1;
        }

    } else {
        /* error case */
        printUsage(argv[1]);
        return -1;
    }
     
    return 0;
}


static void
printUsage(const char *idxname)
{
    printf("%s error. index : server[1] | client[1] request_message [2]).\n", idxname);
}

static int
tcpServ(void)
{
    int fd, openFlag, sockfd, rtn, peer, i;
    mode_t filePerms;
    char rwbuf[RW_BUF_SIZE];    // receive & write buffer
    ssize_t rbyte = 0;          // receive byte
    ssize_t wbyte = 0;          // write byte 
    struct sockaddr_un addr;

    memset(rwbuf, 0x00, sizeof(rwbuf));
    memset(&addr, 0x00, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    openFlag = O_CREAT | O_WRONLY | O_APPEND;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // 0644, rw_-r__-r__

    fd = open("requestedMessage", openFlag, filePerms);
    if (fd == -1) {
        perror("open() \n");
        return -1;
    }

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket() \n");
        return -1;
    }

    printf("After socket() \n");

    rtn = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (rtn == -1) {
        perror("bind() \n");
        goto err;
    }

    printf("After bind() \n");

    rtn = listen(sockfd, 5);
    if (rtn == -1) {
        perror("bind() \n");
        goto err;
    }

    printf("After listen() \n");

    peer = accept(sockfd, NULL, NULL);
    if (peer < 0) {
        perror("accept() \n");
        goto err;
    }

    printf("After accept() \n");

     while ((rbyte = recv(peer, rwbuf, RW_BUF_SIZE, 0)) >= 0) {
        /* receive request */
        if (rbyte == 0) {                       // End of request message!
            break; 
        } else {                                // Partial receive! or Garbage byte occur!
            for (i=rbyte ; i<RW_BUF_SIZE ; i++) {
                rwbuf[i] = '\0';                // must not '\n'
            }
        }

        wbyte = write(fd, rwbuf, strlen(rwbuf));
        if (wbyte == -1) {                      // Error case!
            perror("write() \n");
            goto file_err;
        }
        // } else if (wbyte == 0) {                // Nothing to write!
        //     break;
        // } else if (wbyte != sizeof(rwbuf)) {    // Partial write!
        //     break;
        // }

        printf("%s", rwbuf);
    }

    rtn = close(sockfd);
    if (rtn == -1) {
        perror("close() \n");
        return -1;
    }

    rtn = close(peer);
    if (rtn == -1) {
        perror("close() \n");
        return -1;
    }

    return 0;

err :
    rtn = close(sockfd);
    if (rtn == -1) perror("close() \n");

    rtn = close(fd);
    if (fd == -1) perror("close() \n");

    return -1;

file_err :
    rtn = close(sockfd);
    if (rtn == -1) perror("close() \n");

    rtn = close(peer);
    if (rtn == -1) perror("close() \n");

    rtn = close(fd);
    if (fd == -1) perror("close() \n");

    return -1;
}

static int
tcpClie(const char *request)
{
    int sockfd, rtn;
    ssize_t result;
    struct sockaddr_un addr;

    memset(&addr, 0x00, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket() \n");
        return -1;
    }

    printf("After socket() \n");

    rtn = connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (rtn == -1) {
        perror("bind() \n");
        rtn = close(sockfd);
        if (rtn == -1) {
            perror("close() \n");
        }
        return -1;
    }

    printf("After connect() \n");

    result = send(sockfd, request, strlen(request), 0);

    rtn = close(sockfd);
    if (rtn == -1) {
        perror("close() \n");
        return -1;
    }

    return 0;
}