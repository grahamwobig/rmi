//|--------------------------------------------------------------------------
//|
//|Usage:
//|  The class auto_thread_sync provides synchonization through a combination
//|  of its class constructor and class destructor. To use this class to protect
//|  data in another class one has to declare a STATIC instance of the class
//|  pthread_mutex_wrapper inside the other class and make this instance visible
//|  to all of the methods that need protection. In every place that needs
//|  protection, one should declare an instance of auto_thread_sync. The
//|  constructor for auto_thread_sync requires a boolean value to indicate
//|  whether a read lock or a write lock is needed. When the instance of
//|  auto_thread_sync goes out of scope, its destructor will be called, and this
//|  in turn will unlock the mutex.
//|
//|--------------------------------------------------------------------------

#ifndef AUTO_THREAD_SYNC_H
#define AUTO_THREAD_SYNC_H 1

#ifdef DTHREADIO
#include <iostream>
#endif
#include <pthread.h>

class pthread_mutex_wrapper 
{
public :

    pthread_mutex_wrapper();
    ~pthread_mutex_wrapper();
    pthread_mutex_wrapper(pthread_mutex_wrapper & handle);

    int lock_for_reading();
    int lock_for_writing();

    int unlock_reader();
    int unlock_writer();

    bool get_thread_is_writing() const;
    int get_number_of_writers() const;
    int get_number_of_readers() const;

    const pthread_mutex_t * get_mutex() const;
    const pthread_cond_t * get_lock_free() const;


private :

    bool a_thread_is_writing;
    int number_of_readers_reading;
    int number_of_writers_waiting;
    pthread_mutex_t * mutex;
    pthread_cond_t * lock_free;

    // Do not allow assignment
    pthread_mutex_wrapper & operator=(const pthread_mutex_wrapper & source);
};



class auto_thread_sync {
public :

    auto_thread_sync(pthread_mutex_wrapper & handle, const bool is_writer);
    ~auto_thread_sync();

    #ifdef DTHREADIO
       void print(std::ostream *os) const;
    #endif

private :

    // Do not allow assignment
    void operator=(auto_thread_sync & source);
    
    // Do not alloy copy
    auto_thread_sync(auto_thread_sync & source);
    
    // the incoming static handle
    pthread_mutex_wrapper* theStaticHandle;
    
    bool my_thread_is_writing;

    #ifdef DTHREADIO
       void print_without_lock(std::ostream *os, const char *const title) const;
    #endif
};


#ifdef DTHREADIO
std::ostream &
operator<<(std::ostream &os, auto_thread_sync &reader_writer);
#endif
#endif

