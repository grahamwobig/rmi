
//|--------------------------------------------------------------------------
//|
//|Usage:
//|  The class auto_semaphore_sync provides synchonization through a combination
//|  of its class constructor and class destructor. To use this class to protect
//|  data in another class one has to allocate space for sem_t variable on the
//|  heap. In every place that needs protection, one should declare an instance
//|  of auto_semaphore_sync and pass a pointer to the sem_t variable to the
//|  constructor for auto_semaphore_sync. When the instance of auto_thread_sync
//|  goes out of scope, its destructor will be called, and this in turn will
//|  unlock the semaphore.
//|
//|--------------------------------------------------------------------------

#ifndef AUTO_SEMAPHORE_SYNC_H
#define AUTO_SEMAPHORE_SYNC_H 1


#include <iostream>
#include <semaphore.h>


class auto_semaphore_sync {
public :

    auto_semaphore_sync(sem_t * handle);
    ~auto_semaphore_sync();

    #ifdef DTHREADIO
       void print(std::ostream *os);
    #endif

private :

    // Do not allow assignment
    void operator=(auto_semaphore_sync & source);
    
    // Do not alloy copy
    auto_semaphore_sync(auto_semaphore_sync & source);

    // the incoming static handle
    sem_t * theStaticHandle;

    #ifdef DTHREADIO
       void print_without_lock(std::ostream *os, const char *const title);
    #endif
};

#ifdef DTHREADIO
std::ostream &
operator<<(std::ostream &os, auto_semaphore_sync &writer);
#endif
#endif

