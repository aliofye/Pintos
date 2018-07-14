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


bool
thread_higher_priority (const struct list_elem *a_,
                        const struct list_elem *b_,
                         void *aux UNUSED)
{
  struct thread *a = list_entry (a_, struct thread, elem) ;
  struct thread *b = list_entry (b_, struct thread, elem) ;

  return a->priority > b->priority;
}


/* Returns true if thread a has lower priority than thread b,
 * within a list of threads.
 * (Brian) */
bool
thread_lower_priority (const struct list_elem *a_,
                        const struct list_elem *b_,
                         void *aux UNUSED)
{
  struct thread *a = list_entry (a_, struct thread, elem) ;
  struct thread *b = list_entry (b_, struct thread, elem) ;

  return a->priority < b->priority;
}

bool
thread_donor_priority(const struct list_elem *a_,
                        const struct list_elem *b_,
                          void *aux UNUSED)
{
  struct thread *a = list_entry (a_, struct thread, donationElem);
  struct thread *b = list_entry (b_, struct thread, donationElem);

  return a->priority < b->priority;
}



/* If the ready list contains a thread with a higher priority,
 * yields to it.
 * (Brian) */
void thread_yield_to_higher_priority (void)
{
  enum intr_level old_level = intr_disable ();

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
  intr_set_level (old_level);
}

void recompute_thread_priority (struct thread* t) 
{
   t->priority = 0;
  if(!list_empty(&t->donorList)){
    struct thread *donor = list_entry(list_max(&t->donorList, thread_donor_priority, NULL), struct thread, donationElem);
   
    if (donor->priority > t->priority){
      t->priority = donor->priority;
    }
    else
    {
      if(t->base_priority > t->priority)
       t->priority = t->base_priority;
    }
  }
  else
  {
    t->priority = t->base_priority;
  }

  if (t->donee != NULL)
  {
    recompute_thread_priority(t->donee);
  }
}

void sort_ready_list(void)
{
  list_sort(&ready_list, thread_higher_priority, NULL);
}