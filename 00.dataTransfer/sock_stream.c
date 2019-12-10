#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SOCK_PATH   "sockStreamFile"

static void
print_usage(const char *idxname)
{
    printf("%s error. index : server[1] | client[1]).\n", idxname);
}

static int
server(void)
{
    int fd, openFlag, sockfd, rtn, peer;
    mode_t filePerms;
    char rwbuf[7];              // receive & write buffer
    ssize_t rbyte, wbyte;       // receive byte, write byte 
    struct sockaddr_un addr;

    memset(rwbuf, 0x00, sizeof(rwbuf));
    memset(&addr, 0x00, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    openFlag = O_CREAT | O_WRONLY | O_APPEND;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // 0666, rw_-rw_-rw_

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

    rtn = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (rtn == -1) {
        perror("bind() \n");
        goto err;
    }

    rtn = listen(sockfd, 5);
    if (rtn == -1) {
        perror("bind() \n");
        goto err;
    }

    peer = accept(sockfd, NULL, NULL);
    if (peer < 0) {
        perror("accept() \n");
        goto err;
    }

    while((rbyte = recv(peer, rwbuf, sizeof(rwbuf), 0)) >= 0) {
        if (rbyte != sizeof(rwbuf)) {
            if (rbyte == 0) {
                printf("End of message! rbyte : %ld\n", rbyte);
                break;        
            } else {
                printf("Partial receive occur! rbyte : %ld\n", rbyte);
                break;
            }
        }

        /* write to file */
        wbyte = write(fd, rwbuf, sizeof(rwbuf));
        if (wbyte == -1) {
            perror("write() \n");
            goto file_err;
        } else if (wbyte == 0) {
            printf("Nothing to write! wbyte : %ld\n", wbyte);
        } else if (wbyte > 0 && wbyte != sizeof(rwbuf)) {
            printf("Partial write occur! wbyte : %ld\n", wbyte);
            break;
        }

        printf("%s\n", rwbuf);
    }

    // while (1) {
    //     result = recv(peer, rbuf, sizeof(rbuf), 0);
    //     if (result == -1) {
    //         perror("recv() \n");
    //         rtn = close(sockfd);
    //         goto err;
    //     } else if (result == 0) {
    //         printf("Client's all requests has been received. \n");
    //         break;
    //     } else if (result < sizeof(rbuf)) {
    //         printf("recv() : Partial received. \n");
    //         goto err;
    //     }
    //     return -1;
    // }

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
client(void)
{
    int sockfd, rtn;
    char sbuf[511];
    ssize_t result;
    struct sockaddr_un addr;

    memset(sbuf, 0x00, sizeof(sbuf));
    memset(&addr, 0x00, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket() \n");
        return -1;
    }

    rtn = connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (rtn == -1) {
        perror("bind() \n");
        rtn = close(sockfd);
        if (rtn == -1) {
            perror("close() \n");
        }
        return -1;
    }

    snprintf(sbuf, sizeof(sbuf), "This is a request message from sock_stream!!!\n");
    result = send(sockfd, sbuf, sizeof(sbuf), 0);

    rtn = close(sockfd);
    if (rtn == -1) {
        perror("close() \n");
        return -1;
    }

    return 0;
}

int 
main(int argc, char const *argv[])
{
    int rtn;

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server")) {
        /* server */
        rtn = server();
        if (rtn == -1) {
            perror("server() \n");
            return -1;
        }

    } else if (!strcmp(argv[1], "client")) {
        /* client */
        client();

    } else {
        /* error case */
        print_usage(argv[1]);
        return -1;
    }
    
    return 0;
}
