#import <Foundation/Foundation.h>
#import "NSData-Base64.h"
#import "../Include.h"

static const char base64EncodeLookup[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

@implementation NSData (Base64)

- (NSData *)base64EncodedDataWithOptions:(NSDataBase64EncodingOptions)options
{
    if (self.length < 1) return [NSData data]; // speed optimization

    size_t lineLen = ((options & NSDataBase64Encoding64CharacterLineLength) ? 64 :
                     ((options & NSDataBase64Encoding76CharacterLineLength) ? 76 :
                     0));
    size_t newlineLen = ((options & NSDataBase64EncodingEndLineWithLineFeed) ? 1 : 0) +
                        ((options & NSDataBase64EncodingEndLineWithCarriageReturn) ? 1 : 0);

    if (lineLen != 0 && newlineLen == 0) // default to both if given line len and no newline setting
    {
        options |= NSDataBase64EncodingEndLineWithCarriageReturn | NSDataBase64EncodingEndLineWithLineFeed;
        newlineLen = 2;
    }

    // Length calculation: length in binary units + 1 unit padding if needed,
    //  times size of base64 unit; e.g. 4 bytes Base64 output for every 3 bytes
    //  binary input, rounded up.
    NSUInteger outputLen = (((self.length / 3) + !!(self.length % 3)) << 2);
    outputLen += (lineLen ? (outputLen / lineLen) * newlineLen : 0);
    NSMutableData *outputData = [[NSMutableData alloc] initWithLength:outputLen];
    uint8_t *outBytes = outputData.mutableBytes, _acc_bytes[3] = {0}, *acc_bytes = &_acc_bytes[0];
    __block size_t outpos = 0, outchars = 0, j = 0;

    size_t (^convert)(uint8_t, uint8_t *, uint8_t *, size_t, size_t *) = ^ size_t (uint8_t naccum, uint8_t *accumulated, uint8_t *outbuf, size_t lineLength, size_t *nusedsofar)
    {
        size_t nused = 0;

        NSAssert(naccum < 4, @"Can't accumulate more than 3 bytes at a time!");
        if (naccum > 0)
        {
            outbuf[nused++] =              base64EncodeLookup[((accumulated[0] & 0xfc) >> 2) | 0];
            outbuf[nused++] =              base64EncodeLookup[((accumulated[0] & 0x03) << 4) | (naccum > 1 ? ((accumulated[1] & 0xf0) >> 4) : 0)];
            outbuf[nused++] = naccum > 1 ? base64EncodeLookup[((accumulated[1] & 0x0f) << 2) | (naccum > 2 ? ((accumulated[2] & 0xc0) >> 6) : 0)] : '=';
            outbuf[nused++] = naccum > 2 ? base64EncodeLookup[((accumulated[2] & 0x3f) << 0) | 0] : '=';
        }
        *nusedsofar += 4;
        if (lineLength && (*nusedsofar % lineLength) == 0)
        {
            if ((options & NSDataBase64EncodingEndLineWithCarriageReturn))
            {
                outbuf[nused++] = '\r';
            }
            if ((options & NSDataBase64EncodingEndLineWithLineFeed))
            {
                outbuf[nused++] = '\n';
            }
        }
        return nused;
    };
    [self enumerateByteRangesUsingBlock:(void (^)(const void *, NSRange, BOOL *))^ (const uint8_t *bytes, NSRange byteRange, BOOL *stop)
    {
        for (size_t pos = 0; pos < byteRange.length; ++pos)
        {
            acc_bytes[j++] = bytes[pos];
            if (j >= 3)
            {
                NSAssert(j < 4, @"Can only accumulate 3 bytes at a time!");
                outpos += convert(j, acc_bytes, outBytes + outpos, lineLen, &outchars);
                j = 0;
            }
        }
    }];
    outpos += convert(j, acc_bytes, outBytes + outpos, lineLen, &outchars);
    NSAssert(outpos == outputData.length, @"STOP RIGHT HERE, YOU OVERRAN (or underran) THE BUFFER (expected %u bytes got %zu)", outputData.length, outpos);
    return outputData;
}

- (void)enumerateByteRangesUsingBlock:(void (^)(const void *bytes, NSRange byteRange, BOOL *stop))block
{
    BOOL stop = NO;

    block([self bytes], NSMakeRange(0, [self length]), &stop);
}

#define xx 65 // xx is used to mark invalid Base64 characters
#define EQ 66 // 66 is the sentinel value for the padding '=' character
static uint8_t base64DecodeLookup[256] =
{
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx,
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, 62, xx, xx, xx, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, xx, xx, xx, EQ, xx, xx,
    xx,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, xx, xx, xx, xx, xx,
    xx, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, xx, xx, xx, xx, xx,
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx,
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx,
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx,
    xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx,
};

- (id)initWithBase64EncodedData:(NSData *)base64Data options:(NSDataBase64DecodingOptions)options
{
    // Length calculation: Number of Base64 units rounding up times binary unit
    //  size - e.g. 3 bytes of binary output for every 4 bytes of Base64 input.
    NSUInteger outputLen = ((base64Data.length + 3) >> 2) * 3;
    uint8_t *outbytes = calloc(1, outputLen), _acc_bytes[4] = {0}, *acc_bytes = &_acc_bytes[0];
    if (!outbytes)
    {
        [self release];
        return nil;
    }
    __block NSUInteger outpos = 0, j = 0;

    size_t (^convert)(uint8_t *, size_t, uint8_t *) = ^ size_t (uint8_t *accumulated, size_t naccum, uint8_t *outbuf)
    {
        NSAssert(naccum < 5, @"You can't accumulate more than 4 bytes at a time!");
        for (ssize_t idx = 0; idx < (ssize_t)naccum - 1; ++idx)
        {
            // idx cycles 0,1,2 << 1 == 0,2,4; +2 == 2,4,6; 4- == 4,2,0
            outbuf[idx] = (accumulated[idx] << ((idx << 1) + 2)) | (accumulated[idx + 1] >> (4 - (idx << 1)));
        }
        return naccum ? naccum - 1 : 0;
    };
    [base64Data enumerateByteRangesUsingBlock:(void (^)(const void *, NSRange, BOOL *))^ (const uint8_t *bytes, NSRange byteRange, BOOL *stop)
    {
        for (size_t i = 0; i < byteRange.length; ++i)
        {
            uint8_t decode = base64DecodeLookup[bytes[i]];

            // Die if we're not ignoring unknown characters.
            if (decode == xx && (options & NSDataBase64DecodingIgnoreUnknownCharacters) == 0)
            {
                *stop = YES, j = 1; // use as sentinel
                return;
            }
            else if (decode != EQ && decode != xx) // always ignore padding
            {
                acc_bytes[j++] = decode;
                if (j >= 4)
                {
                    outpos += convert(acc_bytes, j, outbytes + outpos);
                    j = 0; // bzero(acc_bytes, 4);
                }
            }
        }
    }];
    if (j != 1) // success
    {
        outpos += convert(acc_bytes, j, outbytes + outpos); // harmless for j == 0
        NSAssert(outpos <= outputLen, @"Overran the output buffer!"); // outputLen is not exact
        self = [self initWithBytesNoCopy:outbytes length:outpos freeWhenDone:YES];
    }
    else // j == 1 means truncated data or unignored unknown character, both fatal
    {
        free(outbytes);
        [self release];
        self = nil;
    }
    return self;
}

- (id)initWithBase64EncodedString:(NSString *)base64String options:(NSDataBase64DecodingOptions)options
{
    return [self initWithBase64EncodedData:[base64String dataUsingEncoding:NSASCIIStringEncoding] options:options];
}

@end
