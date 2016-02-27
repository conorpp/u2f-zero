
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

int generate_cert(EVP_PKEY * signer, EVP_PKEY ** outpriv, X509 ** outcert)
{
    int ret;
    EC_KEY * key;
    EVP_PKEY * pkey;
    X509 * x509;
    X509_NAME * name;

    key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    pkey = EVP_PKEY_new();
    x509 = X509_new();

    if (key == NULL || pkey == NULL || x509 == NULL)
    {   return 0;  }

    if (!EC_KEY_generate_key(key))
    {   return 0;   }

    if (!EVP_PKEY_assign_EC_KEY(pkey, key))
    {   return 0;  }
    
    if (!ASN1_INTEGER_set(X509_get_serialNumber(x509), 1))
    {   return 0;  }

    if (!X509_gmtime_adj(X509_get_notBefore(x509), 0))
    {   return 0;  }
    if (!X509_gmtime_adj(X509_get_notAfter(x509), 189216000L)) // 6 yrs
    {   return 0;  }
    if (!X509_set_pubkey(x509, pkey))
    {   return 0;  }
    
    name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
            (unsigned char *)"VA", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
            (unsigned char *)"ConorCo LLC", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
            (unsigned char *)"u2fzero.com", -1, -1, 0);

    if (!X509_set_issuer_name(x509, name))
    {   return 0;  }

    if (!X509_sign(x509, pkey, EVP_sha256()))
    {   return 0;  }

    *outpriv = pkey;
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
        fprintf(stderr, "usage: %s <in-privkey> <outcert> <outprivkey>\n", argv[0]);

        return 1;
    }

    X509 * gencert = NULL;
    EVP_PKEY * privkey = NULL, * genprivkey = NULL;
    FILE* f = fopen(argv[1], "r");

    if (f == NULL)
    {
        perror("fopen");
        return 2;
    }

    ERR_load_crypto_strings();

    PEM_read_PrivateKey(f, &privkey, NULL, NULL);

    if (!generate_cert(privkey, &genprivkey, &gencert))
    {   openssl_die();  }

    FILE * fcert;
    fcert = fopen(argv[2], "wb");
    if (fcert == NULL)
    {
        perror("fopen");
        return 2;
    }

    if (!PEM_write_X509(
            fcert,
            gencert
    ))
    { openssl_die(); }


    FILE * fpriv;
    fpriv = fopen(argv[3], "wb");
    if (fpriv == NULL)
    {
        perror("fopen");
        return 2;
    }

    if (!PEM_write_PrivateKey(
            fpriv,
            genprivkey,
            NULL,
            NULL,
            0,
            NULL,
            NULL
    ))
    { openssl_die(); }

    fclose(fcert);
    fclose(fpriv);
    X509_free(gencert);
    EVP_PKEY_free(genprivkey);
    EVP_PKEY_free(privkey);
    ERR_free_strings();
    fclose(f);

    return 0;
}


