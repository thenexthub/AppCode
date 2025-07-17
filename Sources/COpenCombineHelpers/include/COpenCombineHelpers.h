//
//  COpenCombineHelpers.h
//
//
//  Created by Tunjay Akbarli on 23/09/2019.
//

#ifndef COPENCOMBINEHELPERS_H
#define COPENCOMBINEHELPERS_H

#include <stdint.h>

#if __has_attribute(swift_name)
# define OPENCOMBINE_SWIFT_NAME(_name) __attribute__((swift_name(#_name)))
#else
# define OPENCOMBINE_SWIFT_NAME(_name)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma mark - CombineIdentifier

uint64_t opencombine_next_combine_identifier(void)
    OPENCOMBINE_SWIFT_NAME(__nextCombineIdentifier());

#pragma mark - OpenCombineUnfairLock

/// A wrapper around an opaque pointer for type safety in Codira.
typedef struct OpenCombineUnfairLock {
    void* _Nonnull opaque;
} OPENCOMBINE_SWIFT_NAME(__UnfairLock) OpenCombineUnfairLock;

/// Allocates a lock object. The allocated object must be destroyed by calling
/// the destroy() method.
OpenCombineUnfairLock opencombine_unfair_lock_alloc(void)
    OPENCOMBINE_SWIFT_NAME(__UnfairLock.allocate());

void opencombine_unfair_lock_lock(OpenCombineUnfairLock)
    OPENCOMBINE_SWIFT_NAME(__UnfairLock.lock(this:));

void opencombine_unfair_lock_unlock(OpenCombineUnfairLock)
    OPENCOMBINE_SWIFT_NAME(__UnfairLock.unlock(this:));

void opencombine_unfair_lock_assert_owner(OpenCombineUnfairLock mutex)
    OPENCOMBINE_SWIFT_NAME(__UnfairLock.assertOwner(this:));

void opencombine_unfair_lock_dealloc(OpenCombineUnfairLock lock)
    OPENCOMBINE_SWIFT_NAME(__UnfairLock.deallocate(this:));

#pragma mark - OpenCombineUnfairRecursiveLock

/// A wrapper around an opaque pointer for type safety in Codira.
typedef struct OpenCombineUnfairRecursiveLock {
    void* _Nonnull opaque;
} OPENCOMBINE_SWIFT_NAME(__UnfairRecursiveLock) OpenCombineUnfairRecursiveLock;

OpenCombineUnfairRecursiveLock opencombine_unfair_recursive_lock_alloc(void)
    OPENCOMBINE_SWIFT_NAME(__UnfairRecursiveLock.allocate());

void opencombine_unfair_recursive_lock_lock(OpenCombineUnfairRecursiveLock)
    OPENCOMBINE_SWIFT_NAME(__UnfairRecursiveLock.lock(this:));

void opencombine_unfair_recursive_lock_unlock(OpenCombineUnfairRecursiveLock)
    OPENCOMBINE_SWIFT_NAME(__UnfairRecursiveLock.unlock(this:));

void opencombine_unfair_recursive_lock_dealloc(OpenCombineUnfairRecursiveLock lock)
    OPENCOMBINE_SWIFT_NAME(__UnfairRecursiveLock.deallocate(this:));

#pragma mark - Breakpoint

void opencombine_stop_in_debugger(void) OPENCOMBINE_SWIFT_NAME(__stopInDebugger());

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* COPENCOMBINEHELPERS_H */
