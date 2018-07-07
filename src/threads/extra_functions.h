#ifndef THREADS_extra_functions_H
#define THREADS_extra_functions_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h" //HB added 
#include "threads/thread.h"

void donate_priority(struct thread*, struct thread *);
//donate_priority is not implemented anywhere
//the only place that it exists is here so I'm not going to use it 


bool thread_higher_priority (const struct list_elem *a,const struct list_elem *b,void *aux UNUSED);

bool
thread_lower_priority (const struct list_elem *a,const struct list_elem *b,void *aux UNUSED);

void thread_yield_to_higher_priority (void);

bool thread_donor_priority(const struct list_elem *a,const struct list_elem *b,void *aux UNUSED);

bool makeDecisions(const struct list_elem *a,const struct list_elem *b,void *aux UNUSED, bool shouldRightbeHigher, bool isjustelem);

void recompute_thread_priority (struct thread*);

void sort_ready_list(void);

//bool waiter_higher_priority (struct list_elem *_a,struct list_elem *_b,void *aux UNUSED);



#endif