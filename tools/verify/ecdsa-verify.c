
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


int verify(char * digest, char * pubxy, char * rs )
{
    EC_KEY * key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

    BIGNUM * bnx = NULL, * bny = NULL, * bnsig = NULL;

    char r[65], s[65], x[65], y[65];

    if (key == NULL)
    {   return -1;  }

    memmove(x, pubxy, 64);
    memmove(y, pubxy+64, 64);

    r[64] = s[64] = x[64] = y[64] = 0;
    if (!BN_hex2bn(&bnx, x))
    {   return -1;  }
    if (!BN_hex2bn(&bny, y))
    {   return -1;  }

    unsigned char binsig[520];
    if (!BN_hex2bn(&bnsig, rs))
    {   return -1;  }

    int len = BN_bn2bin(bnsig,binsig);

    if (!EC_KEY_set_public_key_affine_coordinates(key,bnx,bny))
    {   return -1;  }

    int ret = ECDSA_verify(0, digest, SHA256_DIGEST_LENGTH, binsig, len, key);

    EC_KEY_free(key);
    BN_free(bnx);
    BN_free(bny);
    BN_free(bnsig);

    printf("returning\n");
    return ret;

}


int main(int argc, char * argv[])
{
    
    char buf[256], c;
    char digest[SHA256_DIGEST_LENGTH];
    int take_digest = 1;

    SHA256_CTX sha256;
    int n, ret;
    int e;

    char * pubkey, * sig;

    if (argc != 3 && argc != 4)
    {
        fprintf(stderr, "usage: %s <public-key-hex> <der-signature-hex> [-d]\n"
                        "   -d: don't take sha256sum of stdin input\n",argv[0]);
        return 1;
    }

    ERR_load_crypto_strings();

    pubkey = argv[1];
    sig = argv[2];

    while ( (c = getopt(argc, argv, "d") ) != -1) 
    {
        switch (c) 
        {
            case 'd':
                take_digest = 0;
                break;
        }
    }

    if (take_digest)
    {
        SHA256_Init(&sha256);

        while ((n = read(STDIN_FILENO, buf, sizeof(buf)))>0)
        {
            SHA256_Update(&sha256, buf, n);
        }

        SHA256_Final(digest, &sha256);
    }
    else
    {
        read(STDIN_FILENO, digest, sizeof(digest));
    }

    ret = verify(digest, 
            pubkey,
            sig);

    switch(ret)
    {
        case -1:
            printf("signature error:\n");
            while((e=ERR_get_error())!=0)
            {
                fprintf(stderr,"%s\n", ERR_error_string(e,NULL) );
            }
            break;
        case 0:
            printf("signature incorrect\n");
            break;
        case 1:
            printf("signature correct\n");
            break;
    }

    ERR_free_strings();
    return 0;
}
