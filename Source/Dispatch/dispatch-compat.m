#import <Foundation/Foundation.h>
#import <pthread.h>
#import <dispatch/dispatch.h>

static CFRunLoopTimerRef dispatch_main_timer;
static bool dispatch_main_timer_running;
static pthread_mutex_t dispatch_main_timer_lock = PTHREAD_MUTEX_INITIALIZER;

static void __dispatch_main_q_pump(CFRunLoopTimerRef timer, void *info) {
    extern struct dispatch_queue_s _dispatch_main_q;
    extern void _dispatch_queue_drain(struct dispatch_queue_s *dq);
    extern bool dispatch_main_q_is_empty(void);

    _dispatch_queue_drain(&_dispatch_main_q);
    if (dispatch_main_q_is_empty()) {
        pthread_mutex_lock(&dispatch_main_timer_lock);
        if (dispatch_main_timer) {
            CFRunLoopRemoveTimer(CFRunLoopGetMain(), dispatch_main_timer, kCFRunLoopCommonModes);
            CFRelease(dispatch_main_timer);
            dispatch_main_timer = NULL;
            dispatch_main_timer_running = false;
        }
        pthread_mutex_unlock(&dispatch_main_timer_lock);
    }
}

static void __start_dispatch_main_timer_if_needed(void) {
    pthread_mutex_lock(&dispatch_main_timer_lock);
    if (!dispatch_main_timer_running) {
        CFRunLoopTimerContext ctx = {0};
        dispatch_main_timer = CFRunLoopTimerCreate(
            NULL,
            CFAbsoluteTimeGetCurrent() + 0.01,  // fire almost immediately
            0.1,                                // repeat every 0.1s
            0, 0,
            __dispatch_main_q_pump,
            &ctx
        );
        CFRunLoopAddTimer(CFRunLoopGetMain(), dispatch_main_timer, kCFRunLoopCommonModes);
        dispatch_main_timer_running = true;
    }
    pthread_mutex_unlock(&dispatch_main_timer_lock);
}

void dispatch_async_(dispatch_queue_t queue, void (^work)(void)) {
    if (!work) return;
    extern struct dispatch_queue_s _dispatch_main_q;
    if (queue == (dispatch_queue_t)&_dispatch_main_q) {
        __start_dispatch_main_timer_if_needed();
    }
    dispatch_async(queue, work);
}
