#include "crypto_aead.h"

#include "spae_aes128_ref.h"

/**
generating a ciphertext c[0],c[1],...,c[*clen-1]
from a plaintext m[0],m[1],...,m[mlen-1]
and associated data ad[0],ad[1],...,ad[adlen-1]
and nonce npub[0],npub[1],...
and secret key k[0],k[1],...
the implementation shall not use nsec
*/
int crypto_aead_encrypt(
    unsigned char *c,unsigned long long *clen,
    const unsigned char *m,unsigned long long mlen,
    const unsigned char *ad,unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k
){
    (void) nsec; /* avoid warning*/
    size_t spae_clen;
    spae_aes128_enc(k,npub,m,mlen,ad,adlen,c,&spae_clen);
    uint8_t padinfo = mlen%SPAE_BLOCKSIZE;
    c[spae_clen] = padinfo;
    *clen = spae_clen+1;
    return 0;
}

/**
the code for the AEAD implementation goes here,
generating a plaintext m[0],m[1],...,m[*mlen-1]
and secret message number nsec[0],nsec[1],...
from a ciphertext c[0],c[1],...,c[clen-1]
and associated data ad[0],ad[1],...,ad[adlen-1]
and nonce number npub[0],npub[1],...
and secret key k[0],k[1],...
*/
int crypto_aead_decrypt(
    unsigned char *m,unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c,unsigned long long clen,
    const unsigned char *ad,unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k
){
    (void) nsec; /* avoid warning*/
    uint8_t padinfo = c[clen-1] & 0x0F;
    size_t spae_mlen = clen-1-SPAE_BLOCKSIZE;
    if(padinfo){
        unsigned int padlen = SPAE_BLOCKSIZE - padinfo;
        spae_mlen -= padlen;
    }
    if(SPAE_PASS!=spae_aes128_dec(k,npub,c,clen-1,ad,adlen,m,spae_mlen))
        return -1;
    *mlen = spae_mlen;
    return 0;
}
