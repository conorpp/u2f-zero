#include <openssl/obj_mac.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>


int main(int argc, char * argv[])
{
    if (argc != 5)
    {
        fprintf(stderr,"usage: %s <lib> <func> <reason> <code>\n",argv[0]);
        exit(1);
    }
    ERR_load_crypto_strings();

    printf("all: %s\n", ERR_error_string(atoi(argv[4]),NULL));
    printf("lib: %s\n", ERR_lib_error_string(atoi(argv[1])));
    printf("func: %s\n", ERR_func_error_string(atoi(argv[2])));
    printf("reason: %s\n", ERR_reason_error_string(atoi(argv[3])));

    ERR_free_strings();
    return 0;
}
