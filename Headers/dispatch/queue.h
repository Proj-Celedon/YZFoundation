/*
 * Copyright (c) 2008-2011 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#ifndef __DISPATCH_QUEUE__
#define __DISPATCH_QUEUE__

#ifndef __DISPATCH_INDIRECT__
#error "Please #include <dispatch/dispatch.h> instead of this file directly."
#include <dispatch/base.h> // for HeaderDoc
#endif

/*!
 * @header
 *
 * Dispatch is an abstract model for expressing concurrency via simple but
 * powerful API.
 *
 * At the core, dispatch provides serial FIFO queues to which blocks may be
 * submitted. Blocks submitted to these dispatch queues are invoked on a pool
 * of threads fully managed by the system. No guarantee is made regarding
 * which thread a block will be invoked on; however, it is guaranteed that only
 * one block submitted to the FIFO dispatch queue will be invoked at a time.
 *
 * When multiple queues have blocks to be processed, the system is free to
 * allocate additional threads to invoke the blocks concurrently. When the
 * queues become empty, these threads are automatically released.
 */

/*!
 * @typedef dispatch_queue_t
 *
 * @abstract
 * Dispatch queues invoke blocks submitted to them serially in FIFO order. A
 * queue will only invoke one block at a time, but independent queues may each
 * invoke their blocks concurrently with respect to each other.
 *
 * @discussion
 * Dispatch queues are lightweight objects to which blocks may be submitted.
 * The system manages a pool of threads which process dispatch queues and
 * invoke blocks submitted to them.
 *
 * Conceptually a dispatch queue may have its own thread of execution, and
 * interaction between queues is highly asynchronous.
 *
 * Dispatch queues are reference counted via calls to dispatch_retain() and
 * dispatch_release(). Pending blocks submitted to a queue also hold a
 * reference to the queue until they have finished. Once all references to a
 * queue have been released, the queue will be deallocated by the system.
 */
DISPATCH_DECL(dispatch_queue);

/*!
 * @typedef dispatch_queue_attr_t
 *
 * @abstract
 * Attribute for dispatch queues.
 */
DISPATCH_DECL(dispatch_queue_attr);

/*!
 * @typedef dispatch_block_t
 *
 * @abstract
 * The prototype of blocks submitted to dispatch queues, which take no
 * arguments and have no return value.
 *
 * @discussion
 * The declaration of a block allocates storage on the stack. Therefore, this
 * is an invalid construct:
 *
 * dispatch_block_t block;
 *
 * if (x) {
 *     block = ^{ printf("true\n"); };
 * } else {
 *     block = ^{ printf("false\n"); };
 * }
 * block(); // unsafe!!!
 *
 * What is happening behind the scenes:
 *
 * if (x) {
 *     struct Block __tmp_1 = ...; // setup details
 *     block = &__tmp_1;
 * } else {
 *     struct Block __tmp_2 = ...; // setup details
 *     block = &__tmp_2;
 * }
 *
 * As the example demonstrates, the address of a stack variable is escaping the
 * scope in which it is allocated. That is a classic C bug.
 */
#ifdef __BLOCKS__
typedef void (^dispatch_block_t)(void);
#endif

__BEGIN_DECLS

/*!
 * @function dispatch_async
 *
 * @abstract
 * Submits a block for asynchronous execution on a dispatch queue.
 *
 * @discussion
 * The dispatch_async() function is the fundamental mechanism for submitting
 * blocks to a dispatch queue.
 *
 * Calls to dispatch_async() always return immediately after the block has
 * been submitted, and never wait for the block to be invoked.
 *
 * The target queue determines whether the block will be invoked serially or
 * concurrently with respect to other blocks submitted to that same queue.
 * Serial queues are processed concurrently with respect to each other.
 *
 * @param queue
 * The target dispatch queue to which the block is submitted.
 * The system will hold a reference on the target queue until the block
 * has finished.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block to submit to the target dispatch queue. This function performs
 * Block_copy() and Block_release() on behalf of callers.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
dispatch_async(dispatch_queue_t queue, dispatch_block_t block);
#endif

/*!
 * @function dispatch_async_f
 *
 * @abstract
 * Submits a function for asynchronous execution on a dispatch queue.
 *
 * @discussion
 * See dispatch_async() for details.
 *
 * @param queue
 * The target dispatch queue to which the function is submitted.
 * The system will hold a reference on the target queue until the function
 * has returned.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_async_f().
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL1 DISPATCH_NONNULL3 DISPATCH_NOTHROW
void
dispatch_async_f(dispatch_queue_t queue,
	void *context,
	dispatch_function_t work);

/*!
 * @function dispatch_sync
 *
 * @abstract
 * Submits a block for synchronous execution on a dispatch queue.
 *
 * @discussion
 * Submits a block to a dispatch queue like dispatch_async(), however
 * dispatch_sync() will not return until the block has finished.
 *
 * Calls to dispatch_sync() targeting the current queue will result
 * in dead-lock. Use of dispatch_sync() is also subject to the same
 * multi-party dead-lock problems that may result from the use of a mutex.
 * Use of dispatch_async() is preferred.
 *
 * Unlike dispatch_async(), no retain is performed on the target queue. Because
 * calls to this function are synchronous, the dispatch_sync() "borrows" the
 * reference of the caller.
 *
 * As an optimization, dispatch_sync() invokes the block on the current
 * thread when possible.
 *
 * @param queue
 * The target dispatch queue to which the block is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block to be invoked on the target dispatch queue.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
dispatch_sync(dispatch_queue_t queue, dispatch_block_t block);
#endif

/*!
 * @function dispatch_sync_f
 *
 * @abstract
 * Submits a function for synchronous execution on a dispatch queue.
 *
 * @discussion
 * See dispatch_sync() for details.
 *
 * @param queue
 * The target dispatch queue to which the function is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_sync_f().
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL1 DISPATCH_NONNULL3 DISPATCH_NOTHROW
void
dispatch_sync_f(dispatch_queue_t queue,
	void *context,
	dispatch_function_t work);

/*!
 * @function dispatch_apply
 *
 * @abstract
 * Submits a block to a dispatch queue for multiple invocations.
 *
 * @discussion
 * Submits a block to a dispatch queue for multiple invocations. This function
 * waits for the task block to complete before returning. If the target queue
 * is concurrent, the block may be invoked concurrently, and it must therefore
 * be reentrant safe.
 *
 * Each invocation of the block will be passed the current index of iteration.
 *
 * @param iterations
 * The number of iterations to perform.
 *
 * @param queue
 * The target dispatch queue to which the block is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block to be invoked the specified number of iterations.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
dispatch_apply(size_t iterations, dispatch_queue_t queue,
		void (^block)(size_t));
#endif

/*!
 * @function dispatch_apply_f
 *
 * @abstract
 * Submits a function to a dispatch queue for multiple invocations.
 *
 * @discussion
 * See dispatch_apply() for details.
 *
 * @param iterations
 * The number of iterations to perform.
 *
 * @param queue
 * The target dispatch queue to which the function is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_apply_f(). The second parameter passed to this function is the
 * current index of iteration.
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL2 DISPATCH_NONNULL4 DISPATCH_NOTHROW
void
dispatch_apply_f(size_t iterations, dispatch_queue_t queue,
	void *context,
	void (*work)(void *, size_t));

/*!
 * @function dispatch_get_current_queue
 *
 * @abstract
 * Returns the queue on which the currently executing block is running.
 *
 * @discussion
 * Returns the queue on which the currently executing block is running.
 *
 * When dispatch_get_current_queue() is called outside of the context of a
 * submitted block, it will return the default concurrent queue.
 *
 * Recommended for debugging and logging purposes only:
 * The code must not make any assumptions about the queue returned, unless it
 * is one of the global queues or a queue the code has itself created.
 * The code must not assume that synchronous execution onto a queue is safe
 * from deadlock if that queue is not the one returned by
 * dispatch_get_current_queue().
 *
 * @result
 * Returns the current queue.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_PURE DISPATCH_WARN_RESULT DISPATCH_NOTHROW
dispatch_queue_t
dispatch_get_current_queue(void);

/*!
 * @function dispatch_get_main_queue
 *
 * @abstract
 * Returns the default queue that is bound to the main thread.
 *
 * @discussion
 * In order to invoke blocks submitted to the main queue, the application must
 * call dispatch_main(), NSApplicationMain(), or use a CFRunLoop on the main
 * thread.
 *
 * @result
 * Returns the main queue. This queue is created automatically on behalf of
 * the main thread before main() is called.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT struct dispatch_queue_s _dispatch_main_q;
#define dispatch_get_main_queue() (&_dispatch_main_q)

/*!
 * @typedef dispatch_queue_priority_t
 * Type of dispatch_queue_priority
 *
 * @constant DISPATCH_QUEUE_PRIORITY_HIGH
 * Items dispatched to the queue will run at high priority,
 * i.e. the queue will be scheduled for execution before
 * any default priority or low priority queue.
 *
 * @constant DISPATCH_QUEUE_PRIORITY_DEFAULT
 * Items dispatched to the queue will run at the default
 * priority, i.e. the queue will be scheduled for execution
 * after all high priority queues have been scheduled, but
 * before any low priority queues have been scheduled.
 *
 * @constant DISPATCH_QUEUE_PRIORITY_LOW
 * Items dispatched to the queue will run at low priority,
 * i.e. the queue will be scheduled for execution after all
 * default priority and high priority queues have been
 * scheduled.
 *
 * @constant DISPATCH_QUEUE_PRIORITY_BACKGROUND
 * Items dispatched to the queue will run at background priority, i.e. the queue
 * will be scheduled for execution after all higher priority queues have been
 * scheduled and the system will run items on this queue on a thread with
 * background status as per setpriority(2) (i.e. disk I/O is throttled and the
 * thread's scheduling priority is set to lowest value).
 */
#define DISPATCH_QUEUE_PRIORITY_HIGH 2
#define DISPATCH_QUEUE_PRIORITY_DEFAULT 0
#define DISPATCH_QUEUE_PRIORITY_LOW (-2)
#define DISPATCH_QUEUE_PRIORITY_BACKGROUND INT16_MIN

typedef long dispatch_queue_priority_t;

/*!
 * @function dispatch_get_global_queue
 *
 * @abstract
 * Returns a well-known global concurrent queue of a given priority level.
 *
 * @discussion
 * The well-known global concurrent queues may not be modified. Calls to
 * dispatch_suspend(), dispatch_resume(), dispatch_set_context(), etc., will
 * have no effect when used with queues returned by this function.
 *
 * @param priority
 * A priority defined in dispatch_queue_priority_t
 *
 * @param flags
 * Reserved for future use. Passing any value other than zero may result in
 * a NULL return value.
 *
 * @result
 * Returns the requested global queue.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_CONST DISPATCH_WARN_RESULT DISPATCH_NOTHROW
dispatch_queue_t
dispatch_get_global_queue(dispatch_queue_priority_t priority,
		unsigned long flags);

/*!
 * @const DISPATCH_QUEUE_SERIAL
 * @discussion A dispatch queue that invokes blocks serially in FIFO order.
 */
#define DISPATCH_QUEUE_SERIAL NULL

/*!
 * @const DISPATCH_QUEUE_CONCURRENT
 * @discussion A dispatch queue that may invoke blocks concurrently and supports
 * barrier blocks submitted with the dispatch barrier API.
 */
#define DISPATCH_QUEUE_CONCURRENT (&_dispatch_queue_attr_concurrent)
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_4_3)
DISPATCH_EXPORT
struct dispatch_queue_attr_s _dispatch_queue_attr_concurrent;

/*!
 * @function dispatch_queue_create
 *
 * @abstract
 * Creates a new dispatch queue to which blocks may be submitted.
 *
 * @discussion
 * Dispatch queues created with the DISPATCH_QUEUE_SERIAL or a NULL attribute
 * invoke blocks serially in FIFO order.
 *
 * Dispatch queues created with the DISPATCH_QUEUE_CONCURRENT attribute may
 * invoke blocks concurrently (similarly to the global concurrent queues, but
 * potentially with more overhead), and support barrier blocks submitted with
 * the dispatch barrier API, which e.g. enables the implementation of efficient
 * reader-writer schemes.
 *
 * When a dispatch queue is no longer needed, it should be released with
 * dispatch_release(). Note that any pending blocks submitted to a queue will
 * hold a reference to that queue. Therefore a queue will not be deallocated
 * until all pending blocks have finished.
 *
 * The target queue of a newly created dispatch queue is the default priority
 * global concurrent queue.
 *
 * @param label
 * A string label to attach to the queue.
 * This parameter is optional and may be NULL.
 *
 * @param attr
 * DISPATCH_QUEUE_SERIAL or DISPATCH_QUEUE_CONCURRENT.
 *
 * @result
 * The newly created dispatch queue.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_MALLOC DISPATCH_WARN_RESULT DISPATCH_NOTHROW
dispatch_queue_t
dispatch_queue_create(const char *label, dispatch_queue_attr_t attr);

/*!
 * @function dispatch_queue_get_label
 *
 * @abstract
 * Returns the label of the queue that was specified when the
 * queue was created.
 *
 * @param queue
 * The result of passing NULL in this parameter is undefined.
 *
 * @result
 * The label of the queue. The result may be NULL.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_PURE DISPATCH_WARN_RESULT
DISPATCH_NOTHROW
const char *
dispatch_queue_get_label(dispatch_queue_t queue);

/*!
 * @const DISPATCH_TARGET_QUEUE_DEFAULT
 * @discussion Constant to pass to the dispatch_set_target_queue() and
 * dispatch_source_create() functions to indicate that the default target queue
 * for the given object type should be used.
 */
#define DISPATCH_TARGET_QUEUE_DEFAULT NULL

/*!
 * @function dispatch_set_target_queue
 *
 * @abstract
 * Sets the target queue for the given object.
 *
 * @discussion
 * An object's target queue is responsible for processing the object.
 *
 * A dispatch queue's priority is inherited from its target queue. Use the
 * dispatch_get_global_queue() function to obtain suitable target queue
 * of the desired priority.
 *
 * Blocks submitted to a serial queue whose target queue is another serial
 * queue will not be invoked concurrently with blocks submitted to the target
 * queue or to any other queue with that same target queue.
 *
 * The result of introducing a cycle into the hierarchy of target queues is
 * undefined.
 *
 * A dispatch source's target queue specifies where its event handler and
 * cancellation handler blocks will be submitted.
 *
 * A dispatch I/O channel's target queue specifies where where its I/O
 * operations are executed.
 *
 * For all other dispatch object types, the only function of the target queue
 * is to determine where an object's finalizer function is invoked.
 *
 * @param object
 * The object to modify.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param queue
 * The new target queue for the object. The queue is retained, and the
 * previous target queue, if any, is released.
 * If queue is DISPATCH_TARGET_QUEUE_DEFAULT, set the object's target queue
 * to the default target queue for the given object type.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NOTHROW // DISPATCH_NONNULL1
void
dispatch_set_target_queue(dispatch_object_t object, dispatch_queue_t queue);

/*!
 * @function dispatch_main
 *
 * @abstract
 * Execute blocks submitted to the main queue.
 *
 * @discussion
 * This function "parks" the main thread and waits for blocks to be submitted
 * to the main queue. This function never returns.
 *
 * Applications that call NSApplicationMain() or CFRunLoopRun() on the
 * main thread do not need to call dispatch_main().
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NOTHROW DISPATCH_NORETURN
void
dispatch_main(void);

/*!
 * @function dispatch_after
 *
 * @abstract
 * Schedule a block for execution on a given queue at a specified time.
 *
 * @discussion
 * Passing DISPATCH_TIME_NOW as the "when" parameter is supported, but not as
 * optimal as calling dispatch_async() instead. Passing DISPATCH_TIME_FOREVER
 * is undefined.
 *
 * @param when
 * A temporal milestone returned by dispatch_time() or dispatch_walltime().
 *
 * @param queue
 * A queue to which the given block will be submitted at the specified time.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block of code to execute.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL2 DISPATCH_NONNULL3 DISPATCH_NOTHROW
void
dispatch_after(dispatch_time_t when,
	dispatch_queue_t queue,
	dispatch_block_t block);
#endif

/*!
 * @function dispatch_after_f
 *
 * @abstract
 * Schedule a function for execution on a given queue at a specified time.
 *
 * @discussion
 * See dispatch_after() for details.
 *
 * @param when
 * A temporal milestone returned by dispatch_time() or dispatch_walltime().
 *
 * @param queue
 * A queue to which the given function will be submitted at the specified time.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_after_f().
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_6,__IPHONE_4_0)
DISPATCH_EXPORT DISPATCH_NONNULL2 DISPATCH_NONNULL4 DISPATCH_NOTHROW
void
dispatch_after_f(dispatch_time_t when,
	dispatch_queue_t queue,
	void *context,
	dispatch_function_t work);

/*!
 * @functiongroup Dispatch Barrier API
 * The dispatch barrier API is a mechanism for submitting barrier blocks to a
 * dispatch queue, analogous to the dispatch_async()/dispatch_sync() API.
 * It enables the implementation of efficient reader/writer schemes.
 * Barrier blocks only behave specially when submitted to queues created with
 * the DISPATCH_QUEUE_CONCURRENT attribute; on such a queue, a barrier block
 * will not run until all blocks submitted to the queue earlier have completed,
 * and any blocks submitted to the queue after a barrier block will not run
 * until the barrier block has completed.
 * When submitted to a a global queue or to a queue not created with the
 * DISPATCH_QUEUE_CONCURRENT attribute, barrier blocks behave identically to
 * blocks submitted with the dispatch_async()/dispatch_sync() API.
 */

/*!
 * @function dispatch_barrier_async
 *
 * @abstract
 * Submits a barrier block for asynchronous execution on a dispatch queue.
 *
 * @discussion
 * Submits a block to a dispatch queue like dispatch_async(), but marks that
 * block as a barrier (relevant only on DISPATCH_QUEUE_CONCURRENT queues).
 *
 * See dispatch_async() for details.
 *
 * @param queue
 * The target dispatch queue to which the block is submitted.
 * The system will hold a reference on the target queue until the block
 * has finished.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block to submit to the target dispatch queue. This function performs
 * Block_copy() and Block_release() on behalf of callers.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_4_3)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
dispatch_barrier_async(dispatch_queue_t queue, dispatch_block_t block);
#endif

/*!
 * @function dispatch_barrier_async_f
 *
 * @abstract
 * Submits a barrier function for asynchronous execution on a dispatch queue.
 *
 * @discussion
 * Submits a function to a dispatch queue like dispatch_async_f(), but marks
 * that function as a barrier (relevant only on DISPATCH_QUEUE_CONCURRENT
 * queues).
 *
 * See dispatch_async_f() for details.
 *
 * @param queue
 * The target dispatch queue to which the function is submitted.
 * The system will hold a reference on the target queue until the function
 * has returned.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_barrier_async_f().
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_4_3)
DISPATCH_EXPORT DISPATCH_NONNULL1 DISPATCH_NONNULL3 DISPATCH_NOTHROW
void
dispatch_barrier_async_f(dispatch_queue_t queue,
	void *context,
	dispatch_function_t work);

/*!
 * @function dispatch_barrier_sync
 *
 * @abstract
 * Submits a barrier block for synchronous execution on a dispatch queue.
 *
 * @discussion
 * Submits a block to a dispatch queue like dispatch_sync(), but marks that
 * block as a barrier (relevant only on DISPATCH_QUEUE_CONCURRENT queues).
 *
 * See dispatch_sync() for details.
 *
 * @param queue
 * The target dispatch queue to which the block is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param block
 * The block to be invoked on the target dispatch queue.
 * The result of passing NULL in this parameter is undefined.
 */
#ifdef __BLOCKS__
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_4_3)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
dispatch_barrier_sync(dispatch_queue_t queue, dispatch_block_t block);
#endif

/*!
 * @function dispatch_barrier_sync_f
 *
 * @abstract
 * Submits a barrier function for synchronous execution on a dispatch queue.
 *
 * @discussion
 * Submits a function to a dispatch queue like dispatch_sync_f(), but marks that
 * fuction as a barrier (relevant only on DISPATCH_QUEUE_CONCURRENT queues).
 *
 * See dispatch_sync_f() for details.
 *
 * @param queue
 * The target dispatch queue to which the function is submitted.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param context
 * The application-defined context parameter to pass to the function.
 *
 * @param work
 * The application-defined function to invoke on the target queue. The first
 * parameter passed to this function is the context provided to
 * dispatch_barrier_sync_f().
 * The result of passing NULL in this parameter is undefined.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_4_3)
DISPATCH_EXPORT DISPATCH_NONNULL1 DISPATCH_NONNULL3 DISPATCH_NOTHROW
void
dispatch_barrier_sync_f(dispatch_queue_t queue,
	void *context,
	dispatch_function_t work);

/*!
 * @functiongroup Dispatch queue-specific contexts
 * This API allows different subsystems to associate context to a shared queue
 * without risk of collision and to retrieve that context from blocks executing
 * on that queue or any of its child queues in the target queue hierarchy.
 */

/*!
 * @function dispatch_queue_set_specific
 *
 * @abstract
 * Associates a subsystem-specific context with a dispatch queue, for a key
 * unique to the subsystem.
 *
 * @discussion
 * The specified destructor will be invoked with the context on the default
 * priority global concurrent queue when a new context is set for the same key,
 * or after all references to the queue have been released.
 *
 * @param queue
 * The dispatch queue to modify.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param key
 * The key to set the context for, typically a pointer to a static variable
 * specific to the subsystem. Keys are only compared as pointers and never
 * dereferenced. Passing a string constant directly is not recommended.
 * The NULL key is reserved and attemps to set a context for it are ignored.
 *
 * @param context
 * The new subsystem-specific context for the object. This may be NULL.
 *
 * @param destructor
 * The destructor function pointer. This may be NULL and is ignored if context
 * is NULL.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_5_0)
DISPATCH_EXPORT DISPATCH_NONNULL1 DISPATCH_NONNULL2 DISPATCH_NOTHROW
void
dispatch_queue_set_specific(dispatch_queue_t queue, const void *key,
	void *context, dispatch_function_t destructor);

/*!
 * @function dispatch_queue_get_specific
 *
 * @abstract
 * Returns the subsystem-specific context associated with a dispatch queue, for
 * a key unique to the subsystem.
 *
 * @discussion
 * Returns the context for the specified key if it has been set on the specified
 * queue.
 *
 * @param queue
 * The dispatch queue to query.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param key
 * The key to get the context for, typically a pointer to a static variable
 * specific to the subsystem. Keys are only compared as pointers and never
 * dereferenced. Passing a string constant directly is not recommended.
 *
 * @result
 * The context for the specified key or NULL if no context was found.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_5_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_PURE DISPATCH_WARN_RESULT
DISPATCH_NOTHROW
void *
dispatch_queue_get_specific(dispatch_queue_t queue, const void *key);

/*!
 * @function dispatch_get_specific
 *
 * @abstract
 * Returns the current subsystem-specific context for a key unique to the
 * subsystem.
 *
 * @discussion
 * When called from a block executing on a queue, returns the context for the
 * specified key if it has been set on the queue, otherwise returns the result
 * of dispatch_get_specific() executed on the queue's target queue or NULL
 * if the current queue is a global concurrent queue.
 *
 * @param key
 * The key to get the context for, typically a pointer to a static variable
 * specific to the subsystem. Keys are only compared as pointers and never
 * dereferenced. Passing a string constant directly is not recommended.
 *
 * @result
 * The context for the specified key or NULL if no context was found.
 */
__OSX_AVAILABLE_STARTING(__MAC_10_7,__IPHONE_5_0)
DISPATCH_EXPORT DISPATCH_NONNULL_ALL DISPATCH_PURE DISPATCH_WARN_RESULT
DISPATCH_NOTHROW
void *
dispatch_get_specific(const void *key);

DISPATCH_EXPORT bool dispatch_main_q_is_empty(void);

__END_DECLS

#endif
