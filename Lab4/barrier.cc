// barrier.cc
//	Implementations of barrier functions.
//
// developed from the following C source code:
//	http://athena.univ-lille1.fr:8888/ab2/coll.45.4/MTP/@Ab2TocView/
//
// Y. Wu, McMaster University, Sept. 2000

#include <thread>
#include <mutex>
#include <condition_variable>
#include "barrier.h"

//--------------------------------------------------------------------------
//Barrier::Barrier 
//	Initialize a barrier for given number of threads.
//
//	"count" is the number of threads involved
//	"type" is one of three types: 
//             USYN_THREAD  :  synchronize threads in the same process. 
//             USYN_PROCESS : synchronize different processes. 
//             USYN_PROCESS_ROBUST : synchronize threads in different processes. 
//	"arg" is reserved for future use.
//--------------------------------------------------------------------------

Barrier::Barrier(int count, int type, void *arg)
{
    maxcnt = count;
    CurrentSb = &sb[0];
    for (int i = 0; i < 2; ++i) {		// construct two subbarriers
        _sb *CurrentSb = &sb[i];
        CurrentSb->runners = count;
        //mutex_init(&CurrentSb->wait_lk, type, arg);
        CurrentSb->wait_lk=new std::mutex;
        //cond_init(&CurrentSb->wait_cv, type, arg);
        CurrentSb->wait_cv=new std::condition_variable;
    }
}

//---------------------------------------------------------------------------
// Barrier::~Barrier
//  	Clean up.
//
//---------------------------------------------------------------------------

Barrier::~Barrier() 
{
    for (int i=0; i < 2; ++ i) {
        //cond_destroy(&sb[i].wait_cv);		// delete condition variables
        delete sb[i].wait_cv;
        //mutex_destroy(&sb[i].wait_lk);		// delete locks
        delete sb[i].wait_lk;
    }

}

//---------------------------------------------------------------------------
// Barrier::Wait
//	Wait untill all threads reach this barrier. The last thread switches
//      "CurrentSb" point to another sub barrier before waking up all waiting
//      threads.
//---------------------------------------------------------------------------

int
Barrier::Wait() 
{
     _sb *TempSb = CurrentSb;		// ptr to a subbarrier
     //mutex_lock(&TempSb->wait_lk);	// acquire lock
     TempSb->wait_lk->lock();

     if (TempSb->runners == 1) {  	// last thread reaching barrier
           if (maxcnt != 1) {
                  TempSb->runners = maxcnt;
					// reset counter
                  CurrentSb = (CurrentSb == &sb[0]) ? &sb[1] : &sb[0];
					// switch the current subbarrier
                 // cond_broadcast(&TempSb->wait_cv);
                  pthread_cond_broadcast((pthread_cond_t*) TempSb->wait_cv);
					// wake up all waiting threads
            }
        }
        else {			// not the last thread
           TempSb->runners--;       	// wait
           while (TempSb->runners != maxcnt)
           {
                  pthread_cond_wait((pthread_cond_t*)TempSb->wait_cv, (pthread_mutex_t*) TempSb->wait_lk);
           }
        }

    // mutex_unlock(&TempSb->wait_lk);    // release lock
       // TempSb->wait_lk->lock();
        TempSb->wait_lk->unlock();

     return(0);
}

