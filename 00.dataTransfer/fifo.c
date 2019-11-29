#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

    fd = open(FIFO_FILENAME, O_WRONLY);
    if (fd == -1) {
        perror("open() \n");
        return -1;
    }

    rtn = write(fd, message, sizeof(message));
    if (rtn = -1) {
        perror("write() \n");
        rtn = close(fd);
        if (rtn == -1) {
            perror("close() \n");
            return -1;
        }
    }

    rtn = close(fd);
    if (rtn == -1) {
        perror("close() \n");
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
        perror("open() \n");
        return -1;
    }

    rtn = read(fd, rbuf, sizeof(rbuf));
    if (rtn = -1) {
        perror("read() \n");
        rtn = close(fd);
        if (rtn == -1) {
            perror("close() \n");
            return -1;
        }
    }

    printf("Writer said... %s \n", rbuf);

    rtn = close(fd);
    if (rtn == -1) {
        perror("close() \n");
        return -1;
    }

    return 0;
}


/* fifo (w|r) */
int 
main(int argc, char const *argv[])
{
    int rtn;

    if (argc < 3) {
        print_usage(argv[0]);
        return -1;
    }

    rtn = unlink(FIFO_FILENAME);
    // if (rtn == -1) {
    //     perror("unlink() \n");
    //     return -1;
    // }

    rtn = mkfifo(FIFO_FILENAME, 0644);
    if (rtn == -1) {
        perror("mkfifo() \n");
        return -1;
    }

    if (argv[1] == "w") {
        /* writer */
        doWrite(argv[2]);
    } else if (argv[1] == "s") {
        /* reader */
        doRead();
    } else {
        print_usage(argv[1]);
        return -1;
    }
    
    return 0;
}
