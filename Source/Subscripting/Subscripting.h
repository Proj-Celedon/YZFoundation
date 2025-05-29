#ifdef __OBJC__
//
//  subscript-shim.h
//  Back-port ObjC Subscripting to iOS 3
//

#import <Foundation/Foundation.h>

// NSArray subscripting
@interface NSArray (Subscripting)
- (id)objectAtIndexedSubscript:(NSUInteger)idx;
@end

// NSMutableArray subscripting
@interface NSMutableArray (Subscripting)
- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx;
@end

// NSDictionary keyed subscripting
@interface NSDictionary (KeyedSubscripting)
- (id)objectForKeyedSubscript:(id)key;
@end

// NSMutableDictionary keyed subscripting
@interface NSMutableDictionary (KeyedSubscripting)
- (void)setObject:(id)obj forKeyedSubscript:(id)key;
@end
#endif
