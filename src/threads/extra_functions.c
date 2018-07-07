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
#ifdef USERPROG
#include "userprog/process.h"
#endif


bool donorUpdate(const struct list_elem *a,const struct list_elem *b)

{
  return makeDecisions(a,b,false,false);
}


bool doesFirstThreadHaveHigherPriority (const struct list_elem *a,const struct list_elem *b)
//
{
	return makeDecisions(a,b, true,true);
  

}



bool doesFirstThreadHaveLowerPriority (const struct list_elem *a,const struct list_elem *b)
{
	return makeDecisions(a,b, false,true);


}

bool makeDecisions(const struct list_elem *a,const struct list_elem *b, bool shouldRightbeHigher, bool isjustelem)
//gets called by doesFirstThreadHaveLowerPriority(), doesFirstThreadHaveHigherPriority(), donorUpdate()
{


  if(shouldRightbeHigher && isjustelem) //means aa should be greater than bb
  {
  	struct thread *aa = list_entry (a, struct thread, elem);
  	struct thread *bb = list_entry (b, struct thread, elem);
  	switch(aa->priority < bb->priority)
  	{
  		case true:
  			return false;
  		case false:
  			break;

  	}
  	return true; 
  }

  if( (!shouldRightbeHigher) &&isjustelem) //means bb should be greater than aa
  {
  	struct thread *aa = list_entry (a, struct thread, elem);
  	struct thread *bb = list_entry (b, struct thread, elem);
  	switch(aa->priority < bb->priority)
  	{
  		case true:
  			return true;
  		case false:
  			break;

  	}
  	return false; 
  }

  if(!isjustelem)
  {
  	struct thread *aa = list_entry (a, struct thread, donationElem);
    struct thread *bb = list_entry (b, struct thread, donationElem);
  	switch(aa->priority > bb->priority)
  	{
  		case false:
  			break;
  		case true:
  			return false; 

  	}
  	return true;
  }


}

void let_higher_go_first (void)
//compares the current thread to the max priority thread 
//and calls thread_yield to select a new thread to run if necessary
{
  enum intr_level old_level = intr_disable ();
  struct thread *c = thread_current ();
  struct thread *m = list_entry (list_max (&ready_list,doesFirstThreadHaveLowerPriority, NULL), struct thread, elem);

  if (!list_empty (&ready_list)) 
  {
    

    if (m->priority > c->priority) {
      if (intr_context ()) {
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
  		printf("");
  }
  intr_set_level (old_level);
}

void newPriority (struct thread* t, void *aux UNUSED)
//uses recursion to make find a thread that the inputted thread will
//donate its priority to using ____ struct 
{
	t->priority = 0;


  if(!list_empty(&t->donorList))
  {
    struct thread *donor = list_entry(list_max(&t->donorList, donorUpdate, NULL), struct thread, donationElem);
   
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
    newPriority(t->donee,NULL); 
  }
  if(t->donee ==NULL)
  {
  	return;
  }
}

void callListSort()
{
	list_sort(&ready_list, doesFirstThreadHaveHigherPriority, NULL);
}


