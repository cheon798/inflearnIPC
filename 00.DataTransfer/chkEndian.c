#include <stdio.h>
#include <stdlib.h>

union byte_long {
    long l;
    unsigned char c[4];
};

union type {
    int i;          // 4 byte
    short int s[2]; // 2 byte x 2 ea
    char c[4];      // 1 byte x 1 ea
};

int 
main(int argc, char const *argv[])
{
    union byte_long bl;

    printf("long    : %ld byte\n", sizeof(long));
    printf("short   : %ld byte\n", sizeof(short));
    printf("int     : %ld byte\n", sizeof(int));
    printf("float   : %ld byte\n", sizeof(float));
    printf("double  : %ld byte\n", sizeof(double));
    printf("char    : %ld byte\n", sizeof(char));

    bl.l = 1200000L;

    printf("(%02x-%02x-%02x-%02x)\n", bl.c[0], bl.c[1], bl.c[2], bl.c[3]);

    bl.l = htonl(bl.l);
    
    printf("(%02x-%02x-%02x-%02x)\n", bl.c[0], bl.c[1], bl.c[2], bl.c[3]);

    union type data;

    data.i = 0x12345678;

    printf("i : 0x%x\ns[0] : 0x%x\ns[1] : 0x%x\nc[0] : 0x%x\nc[1] : 0x%x\nc[2] : 0x%x\nc[3] : 0x%x\n",
           data.i, data.s[0], data.s[1], data.c[0], data.c[1], data.c[2], data.c[3]);

    return 0;
}
