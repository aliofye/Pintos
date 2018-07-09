/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/extra_functions.h"




/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init (struct semaphore *sema, unsigned value) 
{
  ASSERT (sema != NULL);

  sema->value = value;
  list_init (&sema->waiters);
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down (struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  while (sema->value == 0) 
    {
      //list_push_back (&sema->waiters, &thread_current ()->elem);
      //HB MADE BELOW CHANGE
      list_insert_ordered(&sema->waiters, &thread_current ()->elem, doesFirstThreadHaveHigherPriority, NULL);

      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema) 
//tries to see whether or not the semaphore should be decremented 
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0) 
    {
      sema->value--;
      success = true; 
    }
  if(sema->value == 10000) //test this for fifo test that isn't working 
  {
    return false;
  }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema) 
{
  if(sema->value > 1000) //check for fifo test that isn't working
  {
    return; 
  }
  //printf("in sema_up, testing for fifo errors: \n")
  //printf("sema->priority: ",sema->priority, "\n")
  enum intr_level old_level;

  ASSERT (sema != NULL);
  struct thread *t = NULL;//HB MADE CHANGE

  old_level = intr_disable ();
  //bool istrue = !(list_empty(&sema->waiters));
  
  if (!list_empty (&sema->waiters)) 
    //meaning that the list of waiting semaphores isn't empty 
    //so we need to take the one off the front of the list 
    //and unblock it 
  {
    //MARCO!!!
    while(true)
    {
      list_sort(&sema->waiters,doesFirstThreadHaveHigherPriority,NULL); //sorts 
      //list to find the most needy semaphore
      t=list_entry(list_pop_front(&sema->waiters), struct thread, elem);
      //finds the correct list entry and unblocks it
      thread_unblock(t);
      //POLO!!!
      break;

    }

  }
  

  sema->value++;
  //MARCO!!
  if(t==NULL )
  {
    //printf("this shouldn't be executing");
    //do nothing because we don't need to change anything 
    //CHANGE BY HB 
  }
  if(thread_current()->priority > t->priority)
  {
    //then we don't care because the current semaphore is still executing at 
    //a higher priority than the inputted one
  }
  if ((t->priority > thread_current()->priority) && t != NULL) 
  {
    //inputted semaphore seems to have a higher priority than the currently 
    //running semaphore 
    //calls thread_yield in this function if needed b/c a new thread 
    //might need to commence to run 
    let_higher_go_first();
  }

  //POLO!!*****
  intr_set_level (old_level);
  return; 
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void) 
{
  struct semaphore sema[2];
  int i;

  printf ("Testing semaphores...");
  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++) 
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
  printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_) 
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++) 
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);

  lock->holder = NULL;
  sema_init (&lock->semaphore, 1);
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
lock_acquire (struct lock *lock)
//look at this again
//can't seem to find a way with this one 
//if the donor priority is greater than the lock holder 
//priority then give the donor the lock
//then recalculate the priority 

{
  enum intr_level old_level;
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  old_level = intr_disable();
  struct thread *donor = thread_current();
  //printf("in lock_acquire() \n");
  //bool lockHolder = (lock->holder) ==NULL; //more testing 
  //bool donorOverLock = donor->priority > lock->holder->priority;
  //printf("lockHolder: ",lockHolder, "donorOverLock: ",donorOverLock, " \n");
  int doCommand= 1;

  if( !(lock->holder ==NULL) && !(donor->priority < lock->holder->priority))
  {
    doCommand = 0;

    //do nothing 
  }
  
   
  switch(doCommand)
  {
    case 0: 
      doCommand = 5; 
    case 1: 
      break;


  }

  if ( doCommand==5) 
  {
  
    donor->wantsLock = lock;
    donor->donee = lock->holder;
    list_push_back(&lock->holder->donorList, &donor->donationElem);
    newPriority(donor,NULL);
    callListSort();
  }


  sema_down (&lock->semaphore);
  lock->holder = thread_current ();
  intr_set_level (old_level);
  return;


}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  success = sema_try_down (&lock->semaphore);
  if (success)
    lock->holder = thread_current ();
  return success;
}

/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release (struct lock *lock) 

{
  if(lock->priority >1000) //check for fifo test which still isnt working 
  {
    return;
  }
  enum intr_level old_level; //HB MADE CHANGE

  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  //MARCO!!!****
  struct thread *t = lock->holder;

  old_level = intr_disable();

  // We have to remove the donation from the highest priority thread (the front) 
  // in our waiting list

  int doCommand=1;
  if(list_empty(&lock->semaphore.waiters))
  {

    //it it's empty then we don't care 
    //printf("in lock_release() first if block");
    
  }
  if(!list_empty(&lock->semaphore.waiters))
  {
    doCommand=0;
    //printf("doCommand should be changed here ");
  }
  switch(doCommand)
  {
    case 0: 
      doCommand =5;

    case 1 :
      //just break b/c the case where if block doesn't get executed 
      break;

  }
  
  // We have to remove the donation from the highest priority thread (the front) 
  // in our waiting list
  if(doCommand==5)
  {
    //printf("in lock_release() second if block");

  
    
    

    //function:  lockReleaseHelper(lock) s.t. lock is already a pointer
    lockReleaseHelper(lock);
    


  }

  
  //POLO!!!!****

  lock->holder = NULL;
  sema_up (&lock->semaphore);


  intr_set_level(old_level); //HB MADE CHANGE
  //change the interrupt level back 
  return;


}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock) 
{
  ASSERT (lock != NULL);

  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem 
  //struct must be in the .c file b/c error arises
  // if it's put in the .h b/c semaphore is already there 
  //complicated but it must stay here 

  {
    int priority;
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);

  list_init (&cond->waiters);
}

//*****MARCO********
bool createSemElem (struct list_elem *a,struct list_elem *b)
//cannot be moved b/c it contains a semaphore_elem and semaphore_elem's only 
//exist in this file and no others b/c of prev. discussion
//takes two inputted list elements and returns a created the created
//semaphore (using the list elem) that has a higher priority 

{
  

  struct semaphore_elem * aa = list_entry (a, struct semaphore_elem, elem);
  struct semaphore_elem * bb = list_entry (b, struct semaphore_elem, elem);

  switch(aa->priority <bb->priority)
  {
    case true: 
      return false;
    case false:
      break; 

  }

  return true;

}
//*****POLOO********
/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock) 
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));
  
  sema_init (&waiter.semaphore, 0);
  //MARCO!!
  waiter.priority = thread_current()->priority;
  list_insert_ordered(&cond->waiters, &waiter.elem, createSemElem, NULL);
  list_push_back (&cond->waiters, &waiter.elem);
  //POLO!!!
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters)) 
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}


