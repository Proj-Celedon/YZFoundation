//
//  subscript-shim.m
//  Back-port ObjC Subscripting for iOS 3
//

#import <Foundation/Foundation.h>

#pragma mark – NSArray Subscripting

@implementation NSArray (Subscripting)

- (id)objectAtIndexedSubscript:(NSUInteger)idx {
    return [self objectAtIndex:idx];
}

@end

#pragma mark – NSMutableArray Subscripting

@implementation NSMutableArray (Subscripting)

- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx {
    [self replaceObjectAtIndex:idx withObject:obj];
}

@end

#pragma mark – NSDictionary Keyed Subscripting

@implementation NSDictionary (KeyedSubscripting)

- (id)objectForKeyedSubscript:(id)key {
    return [self objectForKey:key];
}

@end

#pragma mark – NSMutableDictionary Keyed Subscripting

@implementation NSMutableDictionary (KeyedSubscripting)

- (void)setObject:(id)obj forKeyedSubscript:(id)key {
    [self setObject:obj forKey:key];
}

@end
