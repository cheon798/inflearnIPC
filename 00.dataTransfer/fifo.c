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
    printf("%s is not proper. select (w|r). \n", idxname);
}

static int
doWrite(const char *message)
{
    int fd, rtn;

    rtn = unlink(FIFO_FILENAME);
    // if (rtn == -1) {
    //     perror("unlink() \n");
    //     return -1;
    // }

    rtn = mkfifo(FIFO_FILENAME, 0644);
    if (rtn == -1) {
        perror("mkfifo() fail!\n");
        return -1;
    }

    fd = open(FIFO_FILENAME, O_WRONLY);
    if (fd == -1) {
        perror("open() fail!\n");
        return -1;
    }

    printf("size of message is %ld \n", sizeof(message));
    rtn = write(fd, message, sizeof(message));
    if (rtn = -1) {
        perror("write() fail!\n");
        rtn = close(fd);
        if (rtn == -1) {
            perror("close() fail!\n");
            return -1;
        }
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
        return -1;
    }

    rtn = read(fd, rbuf, sizeof(rbuf));
    if (rtn = -1) {
        perror("read() fail!\n");
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
    char wbuf[2048];

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "w")) {
        /* writer */
        rtn = unlink(FIFO_FILENAME);

        rtn = mkfifo(FIFO_FILENAME, 0644);
        if (rtn == -1) {
            perror("mkfifo() fail!\n");
            return -1;
        }

        fd = open(FIFO_FILENAME, O_WRONLY);
        if (fd == -1) {
            perror("open() fail!\n");
            return -1;
        }

        strcpy(wbuf, argv[2]);
        printf("size of wbuf is %ld \n", sizeof(wbuf));
        printf("size of argv[2] is %ld \n", sizeof(argv[2]));

        rtn = write(fd, wbuf, sizeof(wbuf));
        if (rtn = -1) {
            perror("write() fail!\n");
            printf("errno is %s \n", strerror(errno));
            rtn = close(fd);
            if (rtn == -1) {
                perror("close() fail!\n");
                return -1;
            }
            return -1;
        }       
    } else if (!strcmp(argv[1], "s")) {
        /* reader */
        doRead();
    } else {
        print_usage(argv[1]);
        return -1;
    }
    
    return 0;
}
