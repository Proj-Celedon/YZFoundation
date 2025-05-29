#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>
#import <CommonCrypto/CommonHMAC.h>

NSData * _Nullable MTPBKDF2(NSData * _Nonnull password, NSData * _Nonnull salt, int rounds);

int CCKeyDerivationPBKDF_shim(
    int algorithm,
    const char * _Nullable password,
    size_t passwordLen,
    const uint8_t * _Nullable salt,
    size_t saltLen,
    int prf,
    uint rounds,
    uint8_t * _Nullable derivedKey,
    size_t derivedKeyLen);
#endif
