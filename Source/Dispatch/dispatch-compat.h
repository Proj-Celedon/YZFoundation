#import <dispatch/dispatch.h>

typedef void (^dispatch_block_t)(void);
typedef struct dispatch_queue_s *dispatch_queue_t;
void dispatch_async_(dispatch_queue_t queue, dispatch_block_t block);
#define dispatch_async dispatch_async_
