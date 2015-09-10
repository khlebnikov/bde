// bdlqq_threadutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLQQ_THREADUTIL
#define INCLUDED_BDLQQ_THREADUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent utilities related to threading.
//
//@CLASSES:
//  bdlqq::ThreadUtil: namespace for portable thread management utilities
//
//@SEE_ALSO: bdlqq_threadattributes, bdlqq_configuration
//
//@DESCRIPTION: This component defines a utility 'struct', 'bdlqq::ThreadUtil',
// that serves as a name space for a suite of pure functions to create threads,
// join them (make one thread block and wait for another thread to exit),
// manipulate thread handles, manipulate the current thread, and (on some
// platforms) access thread-local storage.
//
///Creating a Simple Thread with Default Attributes
///------------------------------------------------
// Clients call 'bdlqq::ThreadUtil::create()' to create threads. Threads
// may be started using a "C" linkage function pointer (of a type defined by
// 'bdlqq::ThreadUtil::ThreadFunction') and a 'void' pointer to 'userData' to
// be passed to the function; or an "invokable" object of parameterized type
// (any copy-constructible type on which 'operator()' may be invoked).  The
// invoked function becomes the main driver for the new thread; when it
// returns, the thread terminates.
//
///Thread Identity
///---------------
// A thread is identified by an object of the opaque type
// 'bdlqq::ThreadUtil::Handle'.  A handle of this type is returned when a
// thread is created (using 'bdlqq::ThreadUtil::create').  A client can also
// retrieve a 'Handle' for the "current" thread via the 'self' method:
//..
//  bdlqq::ThreadUtil::Handle myHandle = bdlqq::ThreadUtil::self();
//..
// Several thread manipulation functions in 'bdlqq::ThreadUtil' take a thread
// handle, or pointer to a thread handle, as an argument.  To facilitate
// compatibility with existing systems and allow for non-portable operations,
// clients also have access to the 'bdlqq::ThreadUtil::NativeHandle' type,
// which exposes the underlying, platform-specific thread identifier type:
//..
//  bdlqq::ThreadUtil::NativeHandle myNativeHandle;
//  myNativeHandle = bdlqq::ThreadUtil::nativeHandle();
//..
// Note that the returned native handle may not be a globally unique
// identifier for the thread, and, e.g., should not be converted to an
// integer identifier, or used as a key in a map.
//
///Setting Thread Priorities
///-------------------------
// 'bdlqq::ThreadUtil' allows clients to configure the priority of newly
// created threads by setting the 'inheritSchedule', 'schedulingPolicy', and
// 'schedulingPriority' of a thread attributes object supplied to the 'create'
// method.  The range of legal values for 'schedulingPriority' depends on both
// the platform and the value of 'schedulingPolicy', and can be obtained from
// the 'getMinSchedulingPriority' and 'getMaxSchedulingPriority' methods.  Both
// 'schedulingPolicy' and 'schedulingPriority' are ignored unless
// 'inheritSchedule' is 'false' (the default value is 'true').  Note that not
// only is effective setting of thread priorities workable on only some
// combinations of platforms and user privileges, but setting the thread policy
// and priority appropriately for one platform may cause thread creation to
// fail on another platform.  Also note that an unset thread priority may be
// interpreted as being outside the valid range defined by
// '[ getMinSchedulingPriority(policy), getMaxSchedulingPriority(policy) ]'.
//..
// Platform      Restrictions
// ------------  --------------------------------------------------------------
// Solaris 5.10  None.
//
// Solaris 5.11  Spawning of threads fails if 'schedulingPolicy' is
//               'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR'.  Note also that
//               'getMinSchedulingPriority' and 'getMaxSchedulingPriority'
//               return different values than on Solaris 5.10.  Thread
//               priorities have so little effect on 5.11 that it is barely
//               detectable.
//
// AIX           For non-privileged clients, spawning of threads fails if
//               'schedulingPolicy' is 'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR'.
//
// Linux         Non-privileged clients *can* *not* make effective use of
//               thread priorities -- spawning of threads fails if
//               'schedulingPolicy' is 'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR',
//               and 'getMinSchedulingPriority == getMaxSchedulingPriority' if
//               the policy has any other value.
//
// HPUX          Non-privileged clients *can* *not* make effective use of
//               thread priorities -- spawning of threads fails if
//               'inheritSchedule' is 'false'.
//
// Darwin        Non-privileged clients *can* *not* make effective use of
//               thread priorities -- there is no observable difference in
//               urgency between high priority and low priority threads.
//               Spawning of threads does succeed, however, for all scheduling
//               policies.
//
// Windows       Clients *can* *not* make effective use of thread priorities --
//               'schedulingPolicy', 'schedulingPriority', and
//               'inheritSchedule' are ignored for all clients.
//..
//
///Supported Clock-Types
///-------------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Creating a Simple Thread with Default Attributes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a thread using the default attribute settings.
// Upon creation, the thread executes the user-supplied C-linkage function
// 'myThreadFunction' that counts 5 seconds before terminating:
//
// First, we create a function that will run in the spawned thread:
//..
//  extern "C" void *myThreadFunction(void *)
//      // Print to standard output "Another second has passed" every second
//      // for five seconds, and return 0.
//  {
//      for (int i = 0; i < 3; ++i) {
//          bdlqq::ThreadUtil::microSleep(0, 1);
//          bsl::cout << "Another second has passed." << bsl::endl;
//      }
//      return 0;
//  }
//..
// Now, we show how to create and join the thread.
//
// After creating the thread, the 'main' routine *joins* the thread, which, in
// effect, causes 'main' to wait for execution of 'myThreadFunction' to
// complete, and guarantees that the output from 'main' will follow the last
// output from the user-supplied function:
//..
//  int main()
//  {
//      bdlqq::Configuration::setDefaultThreadStackSize(
//                  bdlqq::Configuration::recommendedDefaultThreadStackSize());
//
//      bdlqq::ThreadUtil::Handle handle;
//
//      bdlqq::ThreadAttributes attr;
//      attr.setStackSize(1024 * 1024);
//
//      int rc = bdlqq::ThreadUtil::create(&handle, attr, myThreadFunction, 0);
//      assert(0 == rc);
//
//      bdlqq::ThreadUtil::yield();
//
//      rc = bdlqq::ThreadUtil::join(handle);
//      assert(0 == rc);
//
//      bsl::cout << "A three second interval has elapsed\n";
//
//      return 0;
//  }
//..
// Finally, the output of this program is:
//..
//  Another second has passed.
//  Another second has passed.
//  Another second has passed.
//  A three second interval has elapsed.
//..
///Example 2: Creating a Simple Thread with User-Specified Attributes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will choose to override the default thread attribute
// values.
//
// The attributes of a thread can be specified explicitly by supplying a
// 'bdlqq::ThreadAttributes' object to the 'create' method.  For instance, we
// could specify a smaller stack size for a thread to conserve system resources
// if we know that we will require not require the platform's default stack
// size.
//
// First, we define our thread function, noting that it doesn't need much stack
// space:
//..
//  extern "C" void *mySmallStackThreadFunction(void *threadArg)
//      // Initialize a small object on the stack and do some work.
//  {
//      char *initValue = (char *)threadArg;
//      char Small[8];
//      bsl::memset(&Small[0], *initValue, 8);
//      // do some work ...
//      return 0;
//  }
//..
// Finally, we show how to create a detached thread running the function just
// created with a small stack size:
//..
//  void createSmallStackSizeThread()
//      // Create a detached thread with a small stack size and perform some
//      // work.
//  {
//      enum { STACK_SIZE = 16384 };
//      bdlqq::ThreadAttributes attributes;
//      attributes.setDetachedState(
//                             bdlqq::ThreadAttributes::BCEMT_CREATE_DETACHED);
//      attributes.setStackSize(STACK_SIZE);
//
//      char initValue = 1;
//      bdlqq::ThreadUtil::Handle handle;
//      int status = bdlqq::ThreadUtil::create(&handle,
//                                            attributes,
//                                            mySmallStackThreadFunction,
//                                            &initValue);
//  }
//..
//
///Example 3: Setting Thread Priorities
/// - - - - - - - - - - - - - - - - - -
// In this example we demonstrate creating 3 threads with different priorities.
// We use the 'convertToSchedulingPriority' function to translate a normalized,
// floating-point priority in the range '[ 0.0, 1.0 ]' to an integer priority
// in the range '[ getMinSchedulingPriority, getMaxSchedulingPriority ]' to set
// the 'schedulingPriority' attribute.
//..
//  void runSeveralThreads()
//      // Create 3 threads with different priorities and then wait for them
//      // all to finish.
//  {
//      enum { NUM_THREADS = 3 };
//
//      bdlqq::ThreadUtil::Handle handles[NUM_THREADS];
//      bcemt_ThreadFunction functions[NUM_THREADS] = {
//                                                MostUrgentThreadFunction,
//                                                FairlyUrgentThreadFunction,
//                                                LeastUrgentThreadFunction };
//      double priorities[NUM_THREADS] = { 1.0, 0.5, 0.0 };
//
//      bdlqq::ThreadAttributes attributes;
//      attributes.setInheritSchedule(false);
//      const bdlqq::ThreadAttributes::SchedulingPolicy policy =
//                                  bdlqq::ThreadAttributes::BCEMT_SCHED_OTHER;
//      attributes.setSchedulingPolicy(policy);
//
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          attributes.setSchedulingPriority(
//               bdlqq::ThreadUtil::convertToSchedulingPriority(policy,
//                                                             priorities[i]));
//          int rc = bdlqq::ThreadUtil::create(&handles[i],
//                                             attributes,
//                                             functions[i], 0);
//          assert(0 == rc);
//      }
//
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          int rc = bdlqq::ThreadUtil::join(handles[i]);
//          assert(0 == rc);
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLQQ_ENTRYPOINTFUNCTORADAPTER
#include <bdlqq_entrypointfunctoradapter.h>
#endif

#ifndef INCLUDED_BDLQQ_THREADATTRIBUTES
#include <bdlqq_threadattributes.h>
#endif

#ifndef INCLUDED_BDLQQ_THREADUTILIMPL_PTHREAD
#include <bdlqq_threadutilimpl_pthread.h>
#endif

#ifndef INCLUDED_BDLQQ_THREADUTILIMPL_WIN32
#include <bdlqq_threadutilimpl_win32.h>
#endif

#ifndef INCLUDED_BDLQQ_PLATFORM
#include <bdlqq_platform.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BDLF_FUNCTION
#include <bdlf_function.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

extern "C" {
    typedef void *(*bcemt_ThreadFunction)(void *);
        // 'bcemt_ThreadFunction' is an alias for a function type taking a
        // single 'void' pointer argument and returning 'void *'.  Such
        // functions are suitable to be specified as thread entry-point
        // functions to 'bdlqq::ThreadUtil::create'.  Note that 'create' also
        // accepts any invokable C++ "functor" object.

    typedef void (*bcemt_KeyDestructorFunction)(void *);
        // 'bcemt_KeyDestructorFunction' is an alias for a function type taking
        // a single 'void' pointer argument and returning 'void'.  Such
        // functions are suitable to be specified as thread-specific key
        // destructor functions to 'bdlqq::ThreadUtil::createKey'.
}  // extern "C"

namespace bdlqq {

template <class THREAD_POLICY>
struct ThreadUtilImpl;

                             // =================
                             // struct ThreadUtil
                             // =================

struct ThreadUtil {
    // This 'struct' provides a suite of portable utility functions for
    // managing threads.

    // TYPES
    typedef ThreadUtilImpl<Platform::ThreadPolicy> Imp;
        // Platform-specific implementation type.  Do not use directly.

    typedef Imp::Handle                            Handle;
        // Thread handle type.  Use this type to refer to a thread in a
        // platform-independent way.

    typedef Imp::NativeHandle                      NativeHandle;
        // Platform-specific thread handle type.

    typedef Imp::Id                                Id;
        // Thread identifier type - distinguished from a 'Handle' in that
        // it does not have any resources associated with it, whereas 'Handle'
        // may, depending on platform.

    typedef bcemt_ThreadFunction                   ThreadFunction;
        // Prototype for thread entry-point functions.

    typedef Imp::Key                               Key;
        // Thread-specific key type, used to refer to thread-specific storage.

    typedef bcemt_KeyDestructorFunction            Destructor;
        // Prototype for thread-specific key destructors.

public:
    // PUBLIC CLASS METHODS
                // *** Thread Management ***

    static int convertToSchedulingPriority(
              ThreadAttributes::SchedulingPolicy policy,
              double                             normalizedSchedulingPriority);
        // Return an integer scheduling priority appropriate for the specified
        // 'normalizedSchedulingPriority' and the specified 'policy'.  If
        // either the minimum or maximum priority for this platform cannot be
        // determined, return 'ThreadAttributes::BCEMT_UNSET_PRIORITY'.
        // Higher values of 'normalizedSchedulingPriority' are considered to
        // represent more urgent priorities.  The behavior is undefined unless
        // 'policy' is a valid 'ThreadAttributes::SchedulingPolicy' and
        // 'normalizedSchedulingPriority' is in the range '[ 0.0, 1.0 ]'.

    static int create(Handle                  *handle,
                      const ThreadAttributes&  attributes,
                      ThreadFunction           function,
                      void                    *userData);
        // Create a new thread of program control having the specified
        // 'attributes' that invokes the specified 'function' with a single
        // argument specified by 'userData', and load into the specified
        // 'handle' an identifier that may be used to refer to this thread in
        // calls to other 'ThreadUtil' methods.  Return 0 on success, and
        // a non-zero value otherwise.  The behavior is undefined unless
        // 'handle != 0' and unless 'attributes.stackSize' is either greater
        // than 0 or unset.  Note that unless explicitly "detached" (by
        // invoking the 'detach' class method with 'handle') or the
        // 'BCEMT_CREATE_DETACHED' attribute is specified, a call to 'join'
        // must be made to reclaim any system resources associated with the
        // newly-created thread.  Also note that the platform-specific values
        // of default thread stack size vary wildly between platforms; failure
        // to specify a stack size, either through 'attributes' or
        // 'Configuration', can lead to non-portable code.

    static int create(Handle         *handle,
                      ThreadFunction  function,
                      void           *userData);
        // Create a new thread of program control having default attributes
        // (e.g.,  "stack size", "scheduling priority", etc) provided by either
        // 'Configuration' or platform-specific default attributes that
        // invokes the specified 'function' with a single argument specified by
        // 'userData', and load into the specified 'handle' an identifier that
        // may be used to refer to this thread in calls to other
        // 'ThreadUtil' methods.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless 'handle != 0'.
        // Note that unless explicitly "detached" (by invoking
        // 'detach(*handle)'), a call to 'join' must be made to reclaim any
        // system resources associated with the newly-created thread.  Also
        // note that the platform-specific values of default thread stack size
        // vary wildly between platforms; failure to specify a stack size,
        // either through a 'ThreadAttributes' object or through
        // 'Configuration', can lead to non-portable code.

    template <typename INVOKABLE>
    static int create(Handle                  *handle,
                      const ThreadAttributes&  attributes,
                      const INVOKABLE&         function,
                      bslma::Allocator        *allocator = 0);
        // Create a new thread of program control having the specified
        // 'attributes' that invokes the specified 'function' object, and load
        // into the specified 'handle' an identifier that may be used to refer
        // to this thread in calls to other 'ThreadUtil' methods.  Use
        // the specified 'allocator' to create a copy of 'function' to be used
        // by the thread.  If no 'allocator' is specified, the global allocator
        // is used'.  Return 0 on success and a non-zero value otherwise.  The
        // behavior is undefined unless 'handle != 0', unless 'allocator' is
        // thread-safe, and unless 'attributes.stackSize' is either greater
        // than 0 or unset.  'INVOKABLE' shall be a copy-constructible type
        // having the equivalent of 'void operator()()'. Note that unless
        // explicitly "detached" (by invoking 'detach(*handle)') or the
        // 'BCEMT_CREATE_DETACHED' attribute is specified, a call to 'join'
        // must be made to reclaim any system resources associated with the
        // newly-created thread.  Also note that the lifetime of 'allocator',
        // if specified, must exceed the lifetime of the thread.  Also note
        // that the platform-specific values of default thread stack size vary
        // wildly between platforms; failure to specify a stack size, either
        // through 'attributes' or 'Configuration', can lead to non-portable
        // code.

    template <typename INVOKABLE>
    static int create(Handle           *handle,
                      const INVOKABLE&  function,
                      bslma::Allocator *allocator = 0);
        // Create a new thread of program control having default attributes
        // (e.g., "stack size", "scheduling priority", etc) provided by either
        // 'Configuration' or platform-specific default attributes that
        // invokes the specified 'function' object, and load into the specified
        // 'handle' an identifier that may be used to refer to this thread in
        // calls to other 'ThreadUtil' methods.  Use the specified
        // 'allocator' to create a copy of 'function' to be used by the thread.
        // If no 'allocator' is specified, the global allocator is used'.
        // Return 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined unless 'handle != 0' and unless 'allocator' is
        // thread-safe.  'INVOKABLE' shall be a copy-constructible type
        // having the equivalent of 'void operator()()'.  Note that unless
        // explicitly "detached" (by invoking 'detach(*handle)'), a call to
        // 'join' must be made to reclaim any
        // system resources associated with the newly-created thread.  Also
        // note that the lifetime of 'allocator', if specified, must exceed the
        // lifeetime of the thread.  Also note that the platform-specific
        // values of default thread stack size vary wildly between platforms;
        // failure to specify a stack size, either through a 'ThreadAttributes'
        // object or through 'Configuration', can lead to non-portable code.

    static int detach(Handle& handle);
        // "Detach" the thread identified by the specified 'handle' such that
        // when it terminates, the resources associated with that thread will
        // automatically be reclaimed.  The behavior is undefined unless
        // 'handle' was obtained by a call to 'create' or 'self'.  Note that
        // once a thread is "detached", it is no longer possible to 'join' the
        // thread to retrieve its exit status.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If
        // the current thread is not "detached", then a call to 'join' must be
        // made to reclaim any resources used by the thread, and to retrieve
        // the exit status.  Note that the preferred method of exiting a thread
        // is to return from the entry point function.

    static int getMinSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);
        // Return the minimum available priority for the 'policy', where
        // 'policy' is of type 'ThreadAttributes::SchedulingPolicy'.
        // Return 'ThreadAttributes::BCEMT_UNSET_PRIORITY' if the
        // minimum scheduling priority cannot be determined.
        // Note that, for some platform / policy combinations,
        // 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.

    static int getMaxSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);
        // Return the maximum available priority for the 'policy', where
        // 'policy' is of type 'ThreadAttributes::SchedulingPolicy'.
        // Return 'ThreadAttributes::BCEMT_UNSET_PRIORITY' if the
        // maximum scheduling priority cannot be determined.
        // Note that, for some platform / policy combinations,
        // 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.

    static int join(Handle& handle, void **status = 0);
        // Suspend execution of the current thread until the thread referred to
        // by the specified 'handle' terminates, and reclaim any system
        // resources associated with 'handle'.  If the specified 'status' is
        // not 0, load into '*status' the value returned by the function
        // supplied at the creation of the thread identified by 'handle'.  The
        // behavior is undefined unless 'handle' was obtained by a call to
        // 'create'.

    static void microSleep(int microseconds, int seconds = 0);
        // Suspend execution of the current thread for a period of at least the
        // specified 'microseconds' and the optionally specified 'seconds'
        // (relative time).  Note that the actual time suspended depends on
        // many factors including system scheduling and system timer
        // resolution, and may be significantly longer than the time
        // requested.

    static void sleep(const bsls::TimeInterval& time);
        // Suspend execution of the current thread for a period of at least the
        // specified 'time' (relative time).  Note that the actual time
        // suspended depends on many factors including system scheduling and
        // system timer resolution.

    static void sleepUntil(const bsls::TimeInterval&   absoluteTime,
                           bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Suspend execution of the current thread until the specified
        // 'absoluteTime'.  Optionally specify 'clockType' which determines the
        // epoch from which the interval 'absoluteTime' is measured (see
        // {'Supported Clock-Types'} in the component documentation).  The
        // behavior is undefined unless 'absoluteTime' represents a time after
        // January 1, 1970 and before the end of December 31, 9999 (i.e., a
        // time interval greater than or equal to 0, and less than
        // 253,402,300,800 seconds).  Note that the actual time suspended
        // depends on many factors including system scheduling
        // and system timer resolution.

    static void yield();
        // Move the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  Note that this allows cooperating
        // threads of the same priority to share CPU resources equally.

                       // *** Thread Identification ***

    static bool areEqual(const Handle& a, const Handle& b);
        // Return 'true' if the specified 'a' and 'b' thread handles identify
        // the same thread, or if both 'a' and 'b' are invalid handles, and
        // 'false' otherwise.  Note that if *either* of 'a' or 'b' is an
        // invalid handle, but not both, this method returns 'false'.

    static bool areEqualId(const Id& a, const Id& b);
        // Return 'true' if the specified 'a' thread identifier is associated
        // with the same thread as the specified 'b' thread identifier, and
        // 'false' otherwise.

    static Id handleToId(const Handle& threadHandle);
        // Return the unique identifier of the thread having the specified
        // 'threadHandle' within the current process.  The behavior is
        // undefined unless 'handle' was obtained by a call to 'create' or
        // 'self'.  Note that this value is valid only until the thread
        // terminates, and may be reused thereafter.

    static bsls::Types::Uint64 idAsUint64(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // Note that this representation is particularly useful for logging
        // purposes.  Also note that this value is only valid until the thread
        // terminates and may be reused thereafter.

    static int idAsInt(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // Note that this representation is particularly useful for logging
        // purposes.  Also note that this value is only valid until the thread
        // terminates and may be reused thereafter.
        //
        // DEPRECATED: use 'idAsUint64'.

    static const Handle& invalidHandle();
        // Return a reference to the non-modifiable 'Handle' object that is
        // guaranteed never to be a valid thread handle.

    static bool isEqual(const Handle& a, const Handle& b);
        // Return 'true' if the specified 'a' and 'b' thread handles identify
        // the same thread, or if both 'a' and 'b' are invalid handles, and
        // 'false' otherwise.  Note that if *either* of 'a' or 'b' is an
        // invalid handle, but not both, this method returns 'false'.
        //
        // DEPRECATED: use 'areEqual' instead.

    static bool isEqualId(const Id& a, const Id& b);
        // Return 'true' if the specified 'lhs' thread identifier is associated
        // with the same thread as the specified 'rhs' thread identifier, and
        // 'false' otherwise.
        //
        // DEPRECATED: use 'areEqualId' instead.

    static NativeHandle nativeHandle(const Handle& handle);
        // Return the platform-specific identifier associated with the thread
        // referred to by the specified 'handle'.  The behavior is undefined
        // unless 'handle' was obtained by a call to 'create' or 'self'.  Note
        // that the returned native handle may not be a globally unique
        // identifier for the thread (see 'selfIdAsUint').

    static Handle self();
        // Return an identifier that can be used to refer to the current thread
        // in calls to other 'ThreadUtil' methods.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the identifier
        // is only valid until the thread terminates and may be reused
        // thereafter.

    static bsls::Types::Uint64 selfIdAsInt();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  Note that this
        // representation is particularly useful for logging purposes.  Also
        // note that this value is only valid until the thread terminates and
        // may be reused thereafter.
        //
        // DEPRECATED: use 'selfIdAsUint64' instead.

    static bsls::Types::Uint64 selfIdAsUint64();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  Note that this
        // representation is particularly useful for logging purposes.  Also
        // note that this value is valid only until the thread terminates, and
        // may be reused thereafter.

                // *** Thread-Specific (Local) Storage (TSS or TLS) ***

    static int createKey(Key *key, Destructor threadKeyCleanupFunction);
        // Load into the specified 'key' a new process-wide identifier that can
        // be used to store (via 'setSpecific') and retrieve (via
        // 'getSpecific') a pointer value local to each thread, and associate
        // with the new key the specified 'threadKeyCleanupFunction', which
        // will be called by each thread, if 'threadKeyCleanupFunction' is
        // non-zero and the value associated with 'key' for that thread is
        // non-zero, with the associated value as an argument, after the
        // function passed to 'create' has returned and before the thread
        // terminates.  Return 0 on success, and a non-zero value otherwise.
        // Note that multiple keys can be defined, which can result in multiple
        // thread key cleanup functions being called for a given thread.

    static int deleteKey(Key& key);
        // Delete the specified 'key' from the calling process, and
        // disassociate all threads from the thread key cleanup function
        // supplied when 'key' was created (see 'createKey').  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 'key' was obtained from a successful call to 'createKey' and
        // has not already been deleted.  Note that deleting a key does not
        // delete any data referred to by the pointer values associated with
        // that key in any thread.

    static void *getSpecific(const Key& key);
        // Return the thread-local value associated with the specified 'key'.
        // A 'key' is shared among all threads and the value associated with
        // 'key' for each thread is 0 until it is set by that thread using
        // 'setSpecific'.  The behavior is undefined unless this method is
        // called outside any thread key cleanup function associated with any
        // key by 'createKey', 'key' was obtained from a successful call to
        // 'createKey', and 'key' has not been deleted.

    static int setSpecific(const Key& key, const void *value);
        // Associate the specified thread-local 'value' with the specified
        // process-wide 'key'.  Return 0 on success, and a non-zero value
        // otherwise.  The value associated with a thread for a given key is 0
        // until it has been set by that thread using 'setSpecific'.  The
        // behavior is undefined unless this method is called outside any
        // thread key cleanup function associated with any key by 'createKey',
        // 'key' was obtained from a successful call to 'createKey', and 'key'
        // has not been deleted.
};

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct ThreadUtil
                             // -----------------
    
                    // *** Thread Management ***

// CLASS METHODS
inline
int bdlqq::ThreadUtil::create(Handle                  *handle,
                              const ThreadAttributes&  attributes,
                              ThreadFunction           function,
                              void                    *userData)
{
    return Imp::create(handle, attributes, function, userData);
}

inline
int bdlqq::ThreadUtil::create(Handle         *handle,
                              ThreadFunction  function,
                              void           *userData)
{
    return Imp::create(handle, function, userData);
}

template <typename INVOKABLE>
inline
int bdlqq::ThreadUtil::create(Handle                  *handle,
                              const ThreadAttributes&  attributes,
                              const INVOKABLE&         function,
                              bslma::Allocator        *allocator) {
    bslma::ManagedPtr<EntryPointFunctorAdapter<INVOKABLE> > threadData;
    EntryPointFunctorAdapterUtil::allocateAdapter(
                                   &threadData,
                                   function,
                                   bslma::Default::globalAllocator(allocator));

    int rc = create(handle, attributes, bdlqq_EntryPointFunctorAdapter_invoker,
                    threadData.ptr());
    if (0 == rc) {
        threadData.release();
    }
    return rc;
}

template <typename INVOKABLE>
inline
int bdlqq::ThreadUtil::create(Handle           *handle,
                              const INVOKABLE&  function,
                              bslma::Allocator *allocator) {
    bslma::ManagedPtr<EntryPointFunctorAdapter<INVOKABLE> > threadData;
    EntryPointFunctorAdapterUtil::allocateAdapter(
                                   &threadData,
                                   function,
                                   bslma::Default::globalAllocator(allocator));

    int rc = create(handle, bdlqq_EntryPointFunctorAdapter_invoker,
                    threadData.ptr());
    if (0 == rc) {
        threadData.release();
    }
    return rc;
}
    
inline
int bdlqq::ThreadUtil::detach(Handle& handle)
{
    return Imp::detach(handle);
}

inline
void bdlqq::ThreadUtil::exit(void *status)
{
    Imp::exit(status);
}

inline
int bdlqq::ThreadUtil::getMinSchedulingPriority(
                                     ThreadAttributes::SchedulingPolicy policy)
{
    return Imp::getMinSchedulingPriority(policy);
}

inline
int bdlqq::ThreadUtil::getMaxSchedulingPriority(
                                     ThreadAttributes::SchedulingPolicy policy)
{
    return Imp::getMaxSchedulingPriority(policy);
}

inline
int bdlqq::ThreadUtil::join(Handle& thread, void **status)
{
    return Imp::join(thread, status);
}

inline
void bdlqq::ThreadUtil::microSleep(int microseconds, int seconds)
{
    Imp::microSleep(microseconds, seconds);
}

inline
void bdlqq::ThreadUtil::sleep(const bsls::TimeInterval& sleepTime)
{
    Imp::sleep(sleepTime);
}

inline
void bdlqq::ThreadUtil::sleepUntil(const bsls::TimeInterval&   absoluteTime,
                                   bsls::SystemClockType::Enum clockType)
{
    int status = Imp::sleepUntil(absoluteTime, clockType);
    (void) status;  // Suppress a unused variable error.
    BSLS_ASSERT(0 == status);
}


inline
void bdlqq::ThreadUtil::yield()
{
    Imp::yield();
}

                    // *** Thread Identification ***

inline
bool bdlqq::ThreadUtil::areEqual(const Handle& a, const Handle& b)
{
    // Some implementations (notably pthreads) do not define the result of
    // comparing invalid handles.  We avoid undefined behavior by explicitly
    // checking for invalid handles.

    return Imp::INVALID_HANDLE == a
           ? (Imp::INVALID_HANDLE == b)
           : (Imp::INVALID_HANDLE == b ? false : Imp::areEqual(a, b));
}

inline
bool bdlqq::ThreadUtil::areEqualId(const Id& a, const Id& b)
{
    return Imp::areEqualId(a, b);
}

inline
bdlqq::ThreadUtil::Id bdlqq::ThreadUtil::handleToId(const Handle& handle)
{
    return Imp::handleToId(handle);
}


inline
bsls::Types::Uint64 bdlqq::ThreadUtil::idAsUint64(const Id& id)
{
    return Imp::idAsUint64(id);
}


inline
int bdlqq::ThreadUtil::idAsInt(const Id& id)
{
    return Imp::idAsInt(id);
}

inline
const bdlqq::ThreadUtil::Handle& bdlqq::ThreadUtil::invalidHandle()
{
    return Imp::INVALID_HANDLE;
}

inline
bool bdlqq::ThreadUtil::isEqual(const Handle& a, const Handle& b)
{
    return Imp::areEqual(a, b);
}

inline
bool bdlqq::ThreadUtil::isEqualId(const Id& a, const Id& b)
{
    return Imp::areEqualId(a, b);
}

inline
bdlqq::ThreadUtil::NativeHandle
bdlqq::ThreadUtil::nativeHandle(const Handle& handle)
{
    return Imp::nativeHandle(handle);
}

inline
bdlqq::ThreadUtil::Handle bdlqq::ThreadUtil::self()
{
    return Imp::self();
}

inline
bdlqq::ThreadUtil::Id bdlqq::ThreadUtil::selfId()
{
    return Imp::selfId();
}

inline
bsls::Types::Uint64 bdlqq::ThreadUtil::selfIdAsInt()
{
    return Imp::selfIdAsInt();
}

inline
bsls::Types::Uint64 bdlqq::ThreadUtil::selfIdAsUint64()
{
    return Imp::selfIdAsUint64();
}

            // *** Thread-Specific (Local) Storage (TSS or TLS) ***

inline
int bdlqq::ThreadUtil::createKey(Key *key, Destructor threadKeyCleanupFunction)
{
    return Imp::createKey(key, threadKeyCleanupFunction);
}

inline
int bdlqq::ThreadUtil::deleteKey(Key& key)
{
    return Imp::deleteKey(key);
}

inline
void *bdlqq::ThreadUtil::getSpecific(const Key& key)
{
    return Imp::getSpecific(key);
}

inline
int bdlqq::ThreadUtil::setSpecific(const Key& key, const void *value)
{
    return Imp::setSpecific(key, value);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
