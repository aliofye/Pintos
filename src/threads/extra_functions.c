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


bool thread_higher_priority (const struct list_elem *a,const struct list_elem *b,void *aux UNUSED)
//
{
	return makeDecisions(a,b, aux , true,true);
  /*struct thread *aa = list_entry (a, struct thread, elem) ;
  struct thread *bb = list_entry (b, struct thread, elem) ;


  switch(aa->priority < bb->priority)
  {
  	case true:
  		return false;
  	case false:
  		break;

  }
  return true; */


}



bool thread_lower_priority (const struct list_elem *a,const struct list_elem *b,void *aux UNUSED)
{
	return makeDecisions(a,b, aux, false,true);

 /* struct thread *aa = list_entry (a, struct thread, elem) ;
  struct thread *bb = list_entry (b, struct thread, elem) ;

  //return a->priority < b->priority;

  switch(aa->priority > bb->priority)
  {
  	case true: //happens when the aa has a greater priority, which is wrong in 
  		break; //this case because we want it to have lower 
  	case false: 
  		//printf("this happens when the bb thread has a greater priority")
  		return true;
  }
  return false; */


}

bool makeDecisions(const struct list_elem *a,const struct list_elem *b,void *aux UNUSED, bool shouldRightbeHigher, bool isjustelem)
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

bool thread_donor_priority(const struct list_elem *a,const struct list_elem *b,void *aux UNUSED)

{
	return makeDecisions(a,b,aux,false,false);
	/*
  struct thread *aa = list_entry (a, struct thread, donationElem);
  struct thread *bb = list_entry (b, struct thread, donationElem);

  //return a->priority < b->priority;

  switch(aa->priority > bb->priority)
  {
  	case true:
  		break;
  	case false:
  		return true;

  }
  return false;*/
}




void thread_yield_to_higher_priority (void)
//compares the current thread to the max priority thread 
//and calls thread_yield to select a new thread to run if necessary
{
  enum intr_level old_level = intr_disable ();
  struct thread *c = thread_current ();
  struct thread *m = list_entry (list_max (&ready_list,thread_lower_priority, NULL), struct thread, elem);

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

void recompute_thread_priority (struct thread* t )
//uses recursion to make find a thread that the inputted thread will
//donate its priority to using ____ struct 
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

void sort_ready_list()
{
	list_sort(&ready_list, thread_higher_priority, NULL);
}


