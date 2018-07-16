#include "threads/extra_functions.h"
#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
//#include "threads/fixed_point.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

//int listCalls=0;
//extern int x =0


bool greater_than_31 (const struct list_elem *a,const struct list_elem *b)
{
  return makeDecisions(a,b,false,true);
}

bool less_than_31 (const struct list_elem *a,const struct list_elem *b)
{

  return makeDecisions(a,b,false,false);
}

bool donorUpdate(const struct list_elem *a,const struct list_elem *b)
{
  return makeDecisions(a,b,true, true);
}

bool makeDecisions(const struct list_elem *a, const struct list_elem *b, bool isDonationElem, bool isGreater)
{
  //if(isSemElem)
  //{
  //  return
  //}
  if(!isDonationElem)
    //meaning that it's a elem not a donationElem
  {
    struct thread *aa = list_entry (a, struct thread, elem) ;
    struct thread *bb = list_entry (b, struct thread, elem) ;

    switch(isGreater)
    {
      case true:  
        return aa->priority > bb->priority;
      case false: 
        return bb->priority > aa->priority;
    }
  }

  if(isDonationElem)
  {
    struct thread *aa = list_entry (a, struct thread, donationElem);
    struct thread *bb = list_entry (b, struct thread, donationElem);

    return aa->priority > bb->priority;

  }
  else
  {
    return false;
  }

}
void let_higher_go_first (void)
{
  
  enum intr_level old_level = intr_disable ();
  struct thread *c = thread_current ();

  struct thread *m = list_entry (list_max (&ready_list,less_than_31, NULL), struct thread, elem);

  if (!list_empty (&ready_list)) 
  {
    

    if (m->priority > c->priority)
    {
      if (intr_context ()) 
      {
        intr_yield_on_return ();
      }
      else if(!( intr_context() )  )
      {
        thread_yield ();
      }
    }

    
  }
  else if(list_empty(&ready_list))
  {
    if(false)
    {
      thread_yield();
    }
    if(true)
    {
      intr_set_level(old_level);
      return;
    }
      
  }
  intr_set_level (old_level);
}



void newPriority (struct thread* t) 
//input thread is the current thread

{
  t->priority = 0;

  if(!list_empty(&t->donorList))
  {
    struct thread *donor = list_entry(list_max(&t->donorList, donorUpdate, NULL), struct thread, donationElem);
  
    switch(donor->priority > t-> priority )
    {
      case false:
        printf("this shouldn't be exe, in extra_functions \n");
      case true: 
        t->priority = donor->priority;
    }
    switch (t->base_priority > t->priority)
    {
      case true: 
        if(true)
          t->priority = t-> base_priority; 
      case false:
        break;
    } 

  }
  else
  {
    t->priority = t->base_priority;
  }

  if (t->finished != NULL) 
    //computes whether or not the thread t has actually donated anything, if it
    //hasn't then call the function again recursively 
  {
    newPriority(t->finished); //in this case the input thread isn't the current thread
  }
  if(t->finished ==NULL)
  {
    return;
  }
}

void callListSort(void)
{
  list_sort(&ready_list, greater_than_31, NULL);
}

void iterateThrough(struct list_elem * element, struct lock * lock)
{
  struct list_elem * next;
  bool isTrue=true;
  


 while (element != list_end(&thread_current()->donorList))
    {
      isTrue=false;
      struct thread *t = list_entry(element, struct thread, donationElem);
      next = list_next(element);

      switch(!(t->wantsLock==lock))
      {
        case true:
          element=next;
          isTrue=true;
          break;
        case false:
          list_remove(element);
      }
      if(isTrue)
        continue;
      element=next;
    }   
}

bool isEmpty(struct semaphore_elem * a)
//checks if the semaphore list is empty 
{
  if(list_empty(&a->semaphore.waiters))
  {
    return true;  //it is empty so return true
  }
  else 
    return false; //it's not empty 
}

bool semaphoreWaiter (const struct list_elem *a,const struct list_elem *b)
{
  struct semaphore_elem *aa = list_entry(a, struct semaphore_elem, elem);
  struct semaphore_elem *bb = list_entry(b, struct semaphore_elem, elem);
  // Get semaphore with highest waiter priority
  
  if ( isEmpty(bb) )
      return true;

  else if ( isEmpty(aa) )
      return false;
  
  list_sort(&aa->semaphore.waiters, &greater_than_31,NULL);
  list_sort(&bb->semaphore.waiters, &greater_than_31,NULL);
  //listCalls=1;

  return waiterPriority(aa,bb); //returns true if a is the higher of the two priorities of 
  //the threads created with the semaphores 
}