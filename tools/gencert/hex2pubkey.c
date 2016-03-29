//
// hex2pubkey
//
// converts a hex string of (x,y) coordinates
// for a prime256v1 ECC public key to a PEM file
//

#include <openssl/obj_mac.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>


EC_KEY * hex2pubkey(char * pubxy)
{
    EC_KEY * key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

    BIGNUM * bnx = NULL, * bny = NULL;

    char x[65], y[65];

    if (key == NULL)
    {   return NULL;  }

    memmove(x, pubxy, 64);
    memmove(y, pubxy+64, 64);

    x[64] = y[64] = 0;
    
    if (!BN_hex2bn(&bnx, x))
    {   return NULL;  }
    if (!BN_hex2bn(&bny, y))
    {   return NULL;  }

    if (!EC_KEY_set_public_key_affine_coordinates(key,bnx,bny))
    {   return NULL;  }
    
    EC_KEY_set_asn1_flag(key, OPENSSL_EC_NAMED_CURVE);

    BN_free(bnx);
    BN_free(bny);

    return key;
}


int main(int argc, char * argv[])
{
    EC_KEY * key;
    char * pubkey, * out;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <public-key-hex> <output-file>\n"
                        ,argv[0]);
        return 1;
    }

    pubkey = argv[1];
    out = argv[2];

    FILE * fp = fopen(out,"w+");
    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }
    
    ERR_load_crypto_strings();

    key = hex2pubkey(pubkey);

    if (key == NULL)
    {
        fprintf(stderr,"signature error: %s\n", ERR_error_string(ERR_get_error(),NULL) );
        return 1;
    }

    EVP_PKEY * pkey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pkey, key);

    PEM_write_PUBKEY(fp, pkey);
    
    ERR_free_strings();
    EVP_PKEY_free(pkey);

    return 0;
}
