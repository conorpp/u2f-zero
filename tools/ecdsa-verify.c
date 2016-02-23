
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
#include <optarg.h>


int verify(char * digest, char * pubxy, char * rs )
{
    EC_KEY * key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

    ECDSA_SIG sig;
    BIGNUM* bnx = NULL, *bny = NULL;

    char r[65], s[65], x[65], y[65];

    memmove(x, pubxy, 64);
    memmove(y, pubxy+64, 64);
    memmove(r, rs, 64);
    memmove(s, rs+64, 64);
    memset(&sig, 0, sizeof(ECDSA_SIG));

    r[64] = s[64] = x[64] = y[64] = 0;

    if (!BN_hex2bn(&bnx, x))
    {   return -1;  }
    if (!BN_hex2bn(&bny, y))
    {   return -1;  }

    if (!BN_hex2bn(&sig.r, r))
    {   return -1;  }
    if (!BN_hex2bn(&sig.s, s))
    {   return -1;  }

    assert(key != NULL);
    assert(bny != NULL);
    assert(bnx != NULL);

    if (!EC_KEY_set_public_key_affine_coordinates(key,bnx,bny))
    {   return -1;  }

    int ret = ECDSA_do_verify(digest, SHA256_DIGEST_LENGTH, &sig, key);

    EC_KEY_free(key);
    BN_free(bnx);
    BN_free(bny);
    BN_free(sig.r);
    BN_free(sig.s);

    return ret;
}


int main(int argc, char * argv[])
{
    
    char buf[64], c;
    char digest[SHA256_DIGEST_LENGTH];
    char * digest_s = NULL;
    SHA256_CTX sha256;
    int n, ret;

    char * pubkey, * sig;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <public-key-hex> <signature-hex> [-d digest]\n",argv[0]);
        return 1;
    }

    while ( (c = getopt(argc, argv, "i:p:") ) != -1) 
    {
        switch (c) 
        {
            case 'd':
                digest_s = optarg;
                break;
        }
    }

    pubkey = argv[1];
    sig = argv[2];

    if (digest_s == NULL)
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
        digest = digest_s;
    }

    //BIGNUM* bnd = BN_bin2bn(digest, SHA256_DIGEST_LENGTH, NULL);
    //char * digest = BN_bn2hex(bnd);

    ret = verify(digest, 
            pubkey,
            sig);

    switch(ret)
    {
        case -1:
            fprintf(stderr,"signature error: %s\n", ERR_error_string(ERR_get_error(),NULL) );
            break;
        case 0:
            printf("signature incorrect\n");
            break;
        case 1:
            printf("signature correct\n");
            break;
    }

    //BN_free(bnd);
    //OPENSSL_free(digest);
    
    return 0;
}
