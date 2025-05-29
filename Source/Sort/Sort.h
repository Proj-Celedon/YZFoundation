#ifdef __OBJC__
#import <Foundation/Foundation.h>

@interface NSMutableArray (SortShim)
- (void)sortUsingComparator:(NSComparisonResult (^)(id obj1, id obj2))cmptr;
@end

@interface NSArray (SortShim)
- (NSArray *)sortedArrayUsingComparator:(NSComparisonResult (^)(id obj1, id obj2))cmptr;
@end
#endif
