#ifdef __OBJC__
#import <Foundation/Foundation.h>

typedef NS_OPTIONS(NSUInteger, NSDataBase64EncodingOptions) {
    NSDataBase64Encoding64CharacterLineLength = 1UL << 0,
    NSDataBase64Encoding76CharacterLineLength = 1UL << 1,
    NSDataBase64EncodingEndLineWithCarriageReturn = 1UL << 4,
    NSDataBase64EncodingEndLineWithLineFeed = 1UL << 5,
};

typedef NS_OPTIONS(NSUInteger, NSDataBase64DecodingOptions) {
    NSDataBase64DecodingIgnoreUnknownCharacters = 1UL << 0
};

@interface NSData (Base64)

- (NSData *)base64EncodedDataWithOptions:(NSDataBase64EncodingOptions)options;
- (instancetype)initWithBase64EncodedString:(NSString *)base64String
                                    options:(NSDataBase64DecodingOptions)options;
- (instancetype)initWithBase64Encoding:(NSString *)base64String;

@end
#endif
