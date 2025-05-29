#import "CommonCrypto-PBKDF.h"

static void hmac_sha512(const void *key, size_t keyLen,
                        const void *data, size_t dataLen,
                        void *output) {
    CCHmac(kCCHmacAlgSHA512, key, keyLen, data, dataLen, output);
}

NSData * _Nullable MTPBKDF2(NSData * _Nonnull password, NSData * _Nonnull salt, int rounds) {
    if (rounds < 2) return nil;

    const size_t hashLen = CC_SHA512_DIGEST_LENGTH;
    const size_t outputLen = 64;

    NSMutableData *result = [NSMutableData dataWithLength:outputLen];
    uint8_t *outBytes = result.mutableBytes;

    uint8_t U[hashLen];
    uint8_t T[hashLen];

    uint8_t block[4] = {0, 0, 0, 1}; // Block index = 1

    NSMutableData *saltWithBlock = [NSMutableData dataWithData:salt];
    [saltWithBlock appendBytes:block length:4];

    hmac_sha512(password.bytes, password.length, saltWithBlock.bytes, saltWithBlock.length, U);
    memcpy(T, U, hashLen);

    for (int i = 1; i < rounds; i++) {
        hmac_sha512(password.bytes, password.length, U, hashLen, U);
        for (size_t j = 0; j < hashLen; j++) {
            T[j] ^= U[j];
        }
    }

    memcpy(outBytes, T, outputLen); // You can expand this if you want >64 bytes.

    return result;
}

#include <openssl/evp.h>
#include <string.h>

#define kCCPBKDF2 2

#define kCCPRFHmacAlgSHA1   1
#define kCCPRFHmacAlgSHA224 2
#define kCCPRFHmacAlgSHA256 3
#define kCCPRFHmacAlgSHA384 4
#define kCCPRFHmacAlgSHA512 5

#define kCCSuccess 0
#define kCCParamError -4300

int CCKeyDerivationPBKDF(
    int algorithm,
    const char *password,
    size_t passwordLen,
    const uint8_t *salt,
    size_t saltLen,
    int prf,
    uint rounds,
    uint8_t *derivedKey,
    size_t derivedKeyLen
) {
    if (algorithm != kCCPBKDF2 || !password || !salt || !derivedKey || rounds == 0) {
        return kCCParamError;
    }

    const EVP_MD *md = NULL;
    switch (prf) {
        case kCCPRFHmacAlgSHA1:   md = EVP_sha1(); break;
        case kCCPRFHmacAlgSHA224: md = EVP_sha224(); break;
        case kCCPRFHmacAlgSHA256: md = EVP_sha256(); break;
        case kCCPRFHmacAlgSHA384: md = EVP_sha384(); break;
        case kCCPRFHmacAlgSHA512: md = EVP_sha512(); break;
        default:
            return kCCParamError;
    }

    if (!PKCS5_PBKDF2_HMAC(password, (int)passwordLen, salt, (int)saltLen, rounds, md, (int)derivedKeyLen, derivedKey)) {
        return kCCParamError;
    }

    return kCCSuccess;
}
