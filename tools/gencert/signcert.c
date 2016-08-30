//
// gencert.c
//
// Creates a X.509 certificate from input public key 
// and signed by input private key using prime256v1
//


#include <openssl/obj_mac.h>
#include <openssl/ecdsa.h>
#include <openssl/x509.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int generate_cert(EVP_PKEY * signer, EVP_PKEY * pubkey, X509 ** outcert)
{
    int ret;
    X509 * x509, * x509_issuer;
    X509_NAME * name, * issuer_name;

    x509 = X509_new();
    x509_issuer = X509_new();


    if (!ASN1_INTEGER_set(X509_get_serialNumber(x509), 1))
    {   return 0;  }

    if (!X509_gmtime_adj(X509_get_notBefore(x509), 0))
    {   return 0;  }
    if (!X509_gmtime_adj(X509_get_notAfter(x509), 189216000L)) // 6 yrs
    {   return 0;  }
    if (!X509_set_pubkey(x509, pubkey))
    {   return 0;  }
    
    name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
            (unsigned char *)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
            (unsigned char *)"U2F Zero", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
            (unsigned char *)"u2fzero.com", -1, -1, 0);
    
    issuer_name = X509_get_subject_name(x509_issuer);
    X509_NAME_add_entry_by_txt(issuer_name, "C",  MBSTRING_ASC,
            (unsigned char *)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer_name, "ST",  MBSTRING_ASC,
            (unsigned char *)"Some state", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer_name, "L",  MBSTRING_ASC,
            (unsigned char *)"Some city", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer_name, "O",  MBSTRING_ASC,
            (unsigned char *)"Some company", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer_name, "OU",  MBSTRING_ASC,
            (unsigned char *)"Some department", -1, -1, 0);
    X509_NAME_add_entry_by_txt(issuer_name, "CN", MBSTRING_ASC,
            (unsigned char *)"conorpp.com", -1, -1, 0);
 

    if (!X509_set_issuer_name(x509, issuer_name))
    {   return 0;  }

    if (!X509_sign(x509, signer, EVP_sha256()))
    {   return 0;  }

    *outcert = x509;
    return 1;
}

static void openssl_die()
{

    fprintf(stderr,"signature error: %s\n",
            ERR_error_string(ERR_get_error(),NULL) );
    exit(2);
}


int main(int argc, char * argv[])
{

    if (argc != 4)
    {
        fprintf(stderr, "usage: %s <in-signing-key> <in-public-key> <out-cert>\n", argv[0]);
        return 1;
    }

    X509 * gencert = NULL;
    EVP_PKEY * privkey = NULL;
    EVP_PKEY * pubkey = NULL;

    FILE* fpriv = fopen(argv[1], "r");
    FILE* fpub = fopen(argv[2], "r");

    if (fpriv == NULL || fpub == NULL)
    {
        perror("fopen");
        return 2;
    }

    ERR_load_crypto_strings();

    PEM_read_PrivateKey(fpriv, &privkey, NULL, NULL);
    PEM_read_PUBKEY(fpub, &pubkey, NULL, NULL);

    if (!generate_cert(privkey, pubkey, &gencert))
    {   openssl_die();  }

    FILE * fcert;
    fcert = fopen(argv[3], "wb");
    if (fcert == NULL)
    {
        perror("fopen");
        return 2;
    }

    if (!i2d_X509_fp(fcert, gencert))
    { openssl_die(); }


    fclose(fcert);
    X509_free(gencert);
    EVP_PKEY_free(privkey);
    ERR_free_strings();
    fclose(fpriv);

    return 0;
}
