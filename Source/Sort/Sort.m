#import "Sort.h"

@implementation NSMutableArray (SortShim)

typedef NSComparisonResult (^CompareBlock)(id obj1, id obj2);

static NSInteger shim_comparator(id a, id b, void *context) {
    CompareBlock block = (__bridge CompareBlock)context;
    return block(a, b);
}

- (void)sortUsingComparator:(NSComparisonResult (^)(id obj1, id obj2))cmptr {
    if (!cmptr) return;
    [self sortUsingFunction:shim_comparator context:(__bridge void *)cmptr];
}

@end

@implementation NSArray (SortShim)

- (NSArray *)sortedArrayUsingComparator:(NSComparisonResult (^)(id obj1, id obj2))cmptr {
    if (!cmptr) return [self copy];

    return [self sortedArrayUsingFunction:shim_comparator context:(__bridge void *)cmptr];
}

@end
