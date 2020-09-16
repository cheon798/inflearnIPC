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
    if (rtn = -1) {
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
    int rtn;
    char wbuf[1024];

    memset(wbuf, 0x00, sizeof(wbuf));

    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "r")) {		// 두 문자열이 같으면 0, 첫 번째 문자열이 크면 1, 두 번째 문자열이 크면 -1
        /* read */
     
        rtn = doRead();
        if (rtn == -1) {
            perror("doRead() \n");
            return -1;
        }
    } else if (!strcmp(argv[1], "w")) {
        /* write */
     
        rtn = unlink(FIFO_FILENAME);	// 파일을 삭제하는 system call, 정확히 hard link의 이름을 삭제하고 hard link가 참조하는 count를 1 감소시킴
        /*if (rtn == -1) {
            perror("unlink() \n");
            printf("errno is %s \n", strerror(errno));
            return -1;
        }*/

        printf("After unlink() \n");

        rtn = mkfifo(FIFO_FILENAME, 0644);	// pathname의 이름과 mode permission을 갖는 named pipe 생성
        if (rtn == -1) {
            perror("mkfifo() fail!\n");
            return -1;
        }

        printf("After mkfifo(), result : %d \n", rtn);

        strcpy(wbuf, argv[2]);
        printf("wbuf : %s \n", wbuf);
        printf("string length of wbuf is %ld \n", strlen(wbuf));
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
