#ifdef __OBJC__
#import "Subscripting/Subscripting.h"
#endif

// to support numeric literals
#ifdef YES
#undef YES
#undef NO
#endif

#if __has_feature(objc_bool)
#define YES __objc_yes
#define NO  __objc_no
#else
#define YES ((BOOL)1)
#define NO  ((BOOL)0)
#endif

#if __clang__
#define __PRAGMA_PUSH_NO_EXTRA_ARG_WARNINGS _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wformat-extra-args\"")
#define __PRAGMA_POP_NO_EXTRA_ARG_WARNINGS _Pragma("clang diagnostic pop")
#else
#define __PRAGMA_PUSH_NO_EXTRA_ARG_WARNINGS
#define __PRAGMA_POP_NO_EXTRA_ARG_WARNINGS
#endif

#ifdef __OBJC__
#ifdef NSAssert
#undef NSAssert
#define NSAssert(condition, desc, ...) do { \
    __PRAGMA_PUSH_NO_EXTRA_ARG_WARNINGS \
    if (!(condition)) { \
        [[NSAssertionHandler currentHandler] handleFailureInMethod:_cmd \
            object:self file:[NSString stringWithUTF8String:__FILE__] \
            lineNumber:__LINE__ description:(desc), ##__VA_ARGS__]; \
    } \
    __PRAGMA_POP_NO_EXTRA_ARG_WARNINGS \
} while(0)
#endif
#endif

enum {
    NSSortConcurrent = (1UL << 0),
    NSSortStable = (1UL << 4),
};
typedef NSUInteger NSSortOptions;
