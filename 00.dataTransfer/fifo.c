#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define FIFO_FILENAME   "./testfifo"

static void
print_usage(const char *idxname)
{
    printf("%s is not proper. select second index : (w|r). \n", idxname);
}

static int
doWrite(const char *message)
{
    int fd, rtn;

    fd = open(FIFO_FILENAME, O_WRONLY);
    if (fd == -1) {
        perror("open() fail!\n");
        printf("errno is %s \n", strerror(errno));
        return -1;
    }

    printf("After open(), filedescripter : %d\n", fd);
 
    rtn = write(fd, message, strlen(message));
    if (rtn = -1 && (strcmp(strerror(errno), "Success"))) {
        perror("write() fail!\n");
        printf("errno is %s \n", strerror(errno));
        rtn = close(fd);
        if (rtn == -1) {
            perror("close() fail!\n");
            return -1;
        }
        return -1;
    }

    rtn = close(fd);
    if (rtn == -1) {
        perror("close() fail!\n");
        return -1;
    }

    return 0;
}

static int
doRead(void)
{
    int fd, rtn;
    char rbuf[1024];

    memset(rbuf, 0, sizeof(rbuf));

    fd = open(FIFO_FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("open() fail!\n");
        printf("errno is %s \n", strerror(errno));
        return -1;
    }

    printf("After open(), filedescripter : %d\n", fd);

    rtn = read(fd, rbuf, sizeof(rbuf));
    if (rtn = -1 && (strcmp(strerror(errno), "Success"))) {
        perror("read() fail!\n");
        printf("errno is %s \n", strerror(errno));
        rtn = close(fd);
        if (rtn == -1) {
            perror("close() fail!\n");
            return -1;
        }
        return -1;
    }

    printf("Writer said... %s \n", rbuf);

    rtn = close(fd);
    if (rtn == -1) {
        perror("close() fail!\n");
        return -1;
    }

    return 0;
}


/* fifo (w|r) */
int 
main(int argc, char const *argv[])
{
    int fd, rtn;
    char wbuf[1024];
    char rbuf[1024];

    memset(wbuf, 0x00, sizeof(wbuf));
    memset(rbuf, 0x00, sizeof(rbuf));

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "r")) {
        /* read */
     
        strcpy(rbuf, argv[2]);
        printf("rbuf : %s \n", rbuf);
        printf("size of rbuf is %ld \n", strlen(rbuf));
        printf("size of rbuf is %ld \n", sizeof(rbuf));
        printf("size of argv[2] is %ld \n", sizeof(argv[2]));
     
        rtn = doRead();
        if (rtn == -1) {
            perror("doRead() \n");
            return -1;
        }
    } else if (!strcmp(argv[1], "w")) {
        /* write */
     
        rtn = unlink(FIFO_FILENAME);
        if (rtn == -1) {
            perror("unlink() \n");
            printf("errno is %s \n", strerror(errno));
            return -1;
        }

        printf("After unlink() \n");

        rtn = mkfifo(FIFO_FILENAME, 0644);
        if (rtn == -1) {
            perror("mkfifo() fail!\n");
            return -1;
        }

        printf("After mkfifo(), result : %d \n", rtn);

        strcpy(wbuf, argv[2]);
        printf("wbuf : %s \n", wbuf);
        printf("size of wbuf is %ld \n", strlen(wbuf));
        printf("size of wbuf is %ld \n", sizeof(wbuf));
        printf("size of argv[2] is %ld \n", sizeof(argv[2]));

        rtn = doWrite(wbuf);
        if (rtn == -1) {
            perror("doWrite() \n");
            return -1;
        }
    } else {
        print_usage(argv[1]);
        return -1;
    }
    
    return 0;
}
