// ball_defaultobserver.h                                             -*-C++-*-
#ifndef INCLUDED_BALL_DEFAULTOBSERVER
#define INCLUDED_BALL_DEFAULTOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a default observer that emits log records to 'stdout'.
//
//@CLASSES:
//    ball::DefaultObserver: observer that outputs log records to 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@AUTHOR: Banyar Aung
//
//@DESCRIPTION: This component provides a default concrete implementation of
// the 'ball::Observer' protocol for receiving and processing log records:
//..
//               ( ball::DefaultObserver )
//                           |              ctor
//                           |
//                           V
//                   ( ball::Observer )
//                                          dtor
//                                          publish
//..
// 'ball::DefaultObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method
// by printing them to 'stdout'.  Given its minimal functionality,
// 'ball::DefaultObserver' is intended for development use only.
// 'ball::DefaultObserver' is not recommended for use in a production
// environment.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a default
// observer within a 'bael' logging system.
//
// First create a 'ball::DefaultObserver' named 'defaultObserver':
//..
//     ball::DefaultObserver defaultObserver(&bsl::cout);
//..
// The default observer must then be installed within a 'bael' logging system.
// This is done by passing 'defaultObserver' to the 'ball::LoggerManager'
// constructor, which also requires a 'ball::LoggerManagerConfiguration' object.
//..
//     ball::LoggerManagerConfiguration lmc;
//     ball::LoggerManager              loggerManager(&defaultObserver, lmc);
//..
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'defaultObserver'.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {


namespace ball {class Context;
class Record;

                           // ==========================
                           // class DefaultObserver
                           // ==========================

class DefaultObserver : public Observer {
    // This class provides a default implementation of the 'Observer'
    // protocol.  The 'publish' method of this class outputs the log records
    // that it receives to an instance of 'bsl::ostream' that is supplied
    // at construction.

    // DATA
    bsl::ostream *d_stream;  // output sink for log records
    bdlqq::Mutex   d_mutex;   // serializes concurrent calls to 'publish'

    // NOT IMPLEMENTED
    DefaultObserver(const DefaultObserver&);
    DefaultObserver& operator=(const DefaultObserver&);

  public:
    using Observer::publish;

    // CREATORS
    DefaultObserver(bsl::ostream *stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.

    DefaultObserver(bsl::ostream& stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.
        //
        // DEPRECATED: replaced by 'DefaultObserver(bsl::ostream *stream)'

    virtual ~DefaultObserver();
        // Destroy this default observer.

    // MANIPULATORS
    virtual void publish(const Record&  record,
                         const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.
        //
        // Print 'record' and 'context' to the 'bsl::ostream' supplied at
        // construction.  The behavior is undefined if 'record' or 'context' is
        // modified during the execution of this method.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // --------------------------
                           // class DefaultObserver
                           // --------------------------

// CREATORS
inline
DefaultObserver::DefaultObserver(bsl::ostream& stream)
: d_stream(&stream)
{
}

inline
DefaultObserver::DefaultObserver(bsl::ostream *stream)
: d_stream(stream)
{
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
