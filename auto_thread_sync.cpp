//|--------------------------------------------------------------------------
//|Description:
//|  The class auto_thread_sync is intended to be used inside another class
//|  that is operating in a mult-threaded environment. It will synchronize
//|  access to the other class' data.
//|
//|--------------------------------------------------------------------------

#include <iostream>
#include <errno.h>
#include <cstdio>
#include "auto_thread_sync.h"


static const pthread_mutex_t initialized_mutex = PTHREAD_MUTEX_INITIALIZER;
static const pthread_cond_t initialized_cond = PTHREAD_COND_INITIALIZER;


#ifdef DTHREADIO
static const char *boolean_descriptions[] = { "FALSE", "TRUE" };
#endif

static const char * read_write_descriptions[] = { "read", "write" };




pthread_mutex_wrapper::~pthread_mutex_wrapper() 
{
    // Do not do the following.
    // The data these point to will be used in the next auto_thread_sync
    //
    // delete mutex;
    // delete lock_free;
}




pthread_mutex_wrapper::pthread_mutex_wrapper() : a_thread_is_writing(false),
 number_of_readers_reading(0), number_of_writers_waiting(0),
 mutex(new pthread_mutex_t(initialized_mutex)),
 lock_free(new pthread_cond_t(initialized_cond))
  {
    // initialization does it all
}




pthread_mutex_wrapper::pthread_mutex_wrapper
    (pthread_mutex_wrapper & handle)
   : a_thread_is_writing(handle.a_thread_is_writing),
     number_of_readers_reading(handle.number_of_readers_reading),
     number_of_writers_waiting(handle.number_of_writers_waiting),
     mutex(handle.mutex), lock_free(handle.lock_free)
  {
    if (mutex == 0) {
        std::cout <<
            "pthread_mutex_wrapper::copy constructor->found uninitialize mutex"
                 << std::endl;
    }
    
    if (lock_free == 0) {
       std::cout <<
           "pthread_mutex_wrapper::copy constructor->failed to initialize cond"
                 << std::endl;
    }
}





int pthread_mutex_wrapper::lock_for_reading() 
{
    int result = pthread_mutex_lock(mutex);
    
    if (result != 0)
       return result;
    
    while (a_thread_is_writing || (number_of_writers_waiting > 0) ) {
       result = pthread_cond_wait(lock_free, mutex);
       if (result != 0)
          break;
    }
    
    if (result == 0) {
       number_of_readers_reading++;
       result = pthread_mutex_unlock(mutex);
    }
    
    return result;
}





int pthread_mutex_wrapper::lock_for_writing() 
{
    int result = pthread_mutex_lock(mutex);
    
    if (result != 0)
       return result;
    
    number_of_writers_waiting++;
    while (a_thread_is_writing || (number_of_readers_reading > 0) ) {
       result = pthread_cond_wait(lock_free, mutex);
       if (result != 0)
          break;
    }
    
    if (result == 0) {
       a_thread_is_writing = true;
       number_of_writers_waiting--;
       result = pthread_mutex_unlock(mutex);
    }
    
    return result;
}





int pthread_mutex_wrapper::unlock_reader() 
{
    int result = pthread_mutex_lock(mutex);
    
    if (result != 0)
       return result;
    if (number_of_readers_reading < 0) {
       pthread_mutex_unlock(mutex);
       return -1;  // logic error
    }
    
    number_of_readers_reading--;
    result = pthread_cond_broadcast(lock_free);
    
    // do not overwrite a bad result status (only a good one)
    if (result == 0)
       result = pthread_mutex_unlock(mutex);
    else
       pthread_mutex_unlock(mutex); 
    
    return result;
}





int pthread_mutex_wrapper::unlock_writer() 
{
    int result = pthread_mutex_lock(mutex);
    
    if (result != 0)
       return result;
    if (!a_thread_is_writing) {
       pthread_mutex_unlock(mutex);
       return -1;  // logic error
    }
    
    a_thread_is_writing = false;
    result = pthread_cond_broadcast(lock_free);
    
    // do not overwrite a bad result status (only a good one)
    if (result == 0)
       result = pthread_mutex_unlock(mutex);
    else
       pthread_mutex_unlock(mutex); 
    
    return result;
}



bool pthread_mutex_wrapper::get_thread_is_writing() const
{
    return a_thread_is_writing;
}


int pthread_mutex_wrapper::get_number_of_writers() const
{
    return number_of_writers_waiting;
}



int pthread_mutex_wrapper::get_number_of_readers() const
{
    return number_of_readers_reading;
}



const pthread_mutex_t * pthread_mutex_wrapper::get_mutex() const
{
    return mutex;
}



const pthread_cond_t * pthread_mutex_wrapper::get_lock_free() const
{
    return lock_free;
}




auto_thread_sync::auto_thread_sync
    (pthread_mutex_wrapper & handle, const bool is_writer) : 
 
 // Save read/write indication (for destructor)
 my_thread_is_writing(is_writer)
  {
    theStaticHandle = NULL;
    theStaticHandle = &handle;

#ifdef DTHREADIO
    char msg[80];
    snprintf(msg, 80, "Attempting -- %s lock",
             read_write_descriptions[is_writer]);
    print_without_lock(&std::cout, msg);
#endif

    int result = (is_writer)
    
               // chain to base class
               // Note: The following two function calls use their
               //       own internal data. Their internal data was
               //       initialized above with "handle"
               ? theStaticHandle->pthread_mutex_wrapper::lock_for_writing()
               : theStaticHandle->pthread_mutex_wrapper::lock_for_reading();
    
    
    if (result != 0) {
       char message[512];
       snprintf((char *)&message, sizeof(message),
                "Failed to create mutex %s lock ",
                read_write_descriptions[is_writer]);
       std::cout << message << std::endl;
    }
    
// #ifdef DTHREADIO
//     print_without_lock(&std::cout, "constructor -- acquired lock");
// #endif
}





auto_thread_sync::~auto_thread_sync() {

// #ifdef DTHREADIO
//     char msg[80];
//     snprintf(msg, 80, "Attempting to release %s lock",
//              read_write_descriptions[my_thread_is_writing]);
//     print_without_lock(&std::cout, msg);
// #endif

    // Note: the value of "my_thread_is_writing" was initialized in constructor
    int result = (my_thread_is_writing)
    
               // chain to base class
               // Note: The following two function calls use their
               //       own internal data. Their internal data was
               //       initialized in the auto_thread_sync constructor
               ? theStaticHandle->pthread_mutex_wrapper::unlock_writer()
               : theStaticHandle->pthread_mutex_wrapper::unlock_reader();
    
    
    if (result != 0) {
       char message[512];
       snprintf((char *)&message, sizeof(message),
                "Failed to unlock mutex %s lock ",
                read_write_descriptions[my_thread_is_writing]);
       std::cout << message << std::endl;
    }
    
    my_thread_is_writing = false;
    
#ifdef DTHREADIO
    print_without_lock(&std::cout, "destructor -- unlock");
#endif
}



#ifdef DTHREADIO

void
auto_thread_sync::print_without_lock(std::ostream *os, const char *const title) const
{
   *os << "auto_thread_sync::" << title << std::endl
//        << "a_thread_is_writing: "
//        << boolean_descriptions
//           [ theStaticHandle->get_thread_is_writing() ] << std::endl 
// //        << "my_thread_is_writing: "
// //        << boolean_descriptions[ my_thread_is_writing ] << std::endl 
//        << "mutex ADDRESS: "
//        << theStaticHandle->get_mutex() << std::endl 
//        << "lock_free ADDRESS: "
//        << theStaticHandle->get_lock_free() << std::endl 
// //        << "number_of_readers_reading: "
// //        << theStaticHandle->get_number_of_readers() << std::endl 
// //        << "number_of_writers_waiting: "
// //        << theStaticHandle->get_number_of_writers() << std::endl
       ;
}



void
auto_thread_sync::print(std::ostream *os) const
{
   theStaticHandle->lock_for_reading();

   print_without_lock(os, "print");

   theStaticHandle->unlock_reader();
}




std::ostream &
operator<<(std::ostream &os, auto_thread_sync &reader_writer) {
   reader_writer.print(&os);
   return os;
}

#endif

