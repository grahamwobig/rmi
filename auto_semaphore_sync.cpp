//|--------------------------------------------------------------------------
//|Description:
//|  The class auto_semaphore_sync is intended to be used inside another class
//|  that is operating in a mult-threaded environment. It will synchronize
//|  access to the other class' data.
//|
//|--------------------------------------------------------------------------


#include <errno.h>
#include "auto_semaphore_sync.h"
#include <string.h>



//## class auto_semaphore_sync
auto_semaphore_sync::auto_semaphore_sync(sem_t * handle)
  : theStaticHandle(handle)
{
#ifdef DTHREADIO
    print_without_lock(&std::cout, "constructor begin wait");
#endif
 
    int semStatus = sem_wait(theStaticHandle);
    if (semStatus != 0)
       std::cout <<  "Received non-zero semaphore status: " << std::endl;

#ifdef DTHREADIO
    print_without_lock(&std::cout, "constructor wait complete");
#endif
}



auto_semaphore_sync::~auto_semaphore_sync() {
#ifdef DTHREADIO
    print_without_lock(&std::cout, "destructor begin post");
#endif

    sem_post(theStaticHandle);

#ifdef DTHREADIO
    print_without_lock(&std::cout, "destructor post complete");
#endif
}



#ifdef DTHREADIO

void
auto_semaphore_sync::print_without_lock(std::ostream *os, const char *const title)
{
   *os << "\nauto_semaphore_sync::" << title << std::endl
       << "ADDRESS: "
       << theStaticHandle << std::endl;
}



void
auto_semaphore_sync::print(std::ostream *os)
{
   print_without_lock(os, "print");
}




std::ostream &
operator<<(std::ostream &os, auto_semaphore_sync &writer) {
   writer.print(&os);
   return os;
}
#endif

