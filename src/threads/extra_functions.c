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


bool thread_higher_priority (const struct list_elem *a,const struct list_elem *b)
{
  return makeDecisions(a,b,false,true);
}

bool thread_lower_priority (const struct list_elem *a,const struct list_elem *b)
{

  return makeDecisions(a,b,false,false);
}

bool thread_donor_priority(const struct list_elem *a,const struct list_elem *b)
{
  return makeDecisions(a,b,true,true);
}

bool makeDecisions(const struct list_elem *a, const struct list_elem *b, bool isDonationElem, bool isGreater)
{
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
void thread_yield_to_higher_priority (void)
{
  /*enum intr_level old_level = intr_disable ();

  if (!list_empty (&ready_list))
    //if the list is not empty 
   {
    struct thread *cur = thread_current ();
    struct thread *max = list_entry (list_max (&ready_list,
          thread_lower_priority, NULL), struct thread, elem);
    if (max->priority > cur->priority)

     {
      if (intr_context ()) 

      {
        intr_yield_on_return ();
      }
      else

      {
        thread_yield ();
      }
    }

    
  }
  intr_set_level (old_level); */
  enum intr_level old_level = intr_disable ();
  struct thread *c = thread_current ();
  struct thread *m = list_entry (list_max (&ready_list,thread_lower_priority, NULL), struct thread, elem);

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



void recompute_thread_priority (struct thread* t) 


{
  t->priority = 0;

  if(!list_empty(&t->donorList))
  {
    struct thread *donor = list_entry(list_max(&t->donorList, thread_donor_priority, NULL), struct thread, donationElem);
   
    //if (donor->priority > t->priority){
    //  t->priority = donor->priority;
    //}
    //else
    //{
    //  if(t->base_priority > t->priority)
  //     t->priority = t->base_priority;
    //}
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

  if (t->donee != NULL) 
    //computes whether or not the thread t has actually donated anything, if it
    //hasn't then call the function again recursively 
  {
    recompute_thread_priority(t->donee); 
  }
  if(t->donee ==NULL)
  {
    return;
  }
}

void sort_ready_list(void)
{
  list_sort(&ready_list, thread_higher_priority, NULL);
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

      /*if (t->wantsLock == lock)
       {
        list_remove(element);
       }
      element = next;*/
    }


    
}