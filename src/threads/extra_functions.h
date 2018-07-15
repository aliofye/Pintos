#ifndef THREADS_extra_functions_H
#define THREADS_extra_functions_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h" //HB added 

//extern bool listCalls;



bool thread_higher_priority (const struct list_elem*,
                        const struct list_elem *);

bool thread_lower_priority (const struct list_elem *,const struct list_elem *);

void thread_yield_to_higher_priority (void);

bool thread_donor_priority(const struct list_elem *,const struct list_elem *);

void recompute_thread_priority (struct thread*);

void sort_ready_list(void);

bool makeDecisions(const struct list_elem *a, const struct list_elem *b, bool isDonationElem, bool isGreater);

void iterateThrough(struct list_elem * element, struct lock * lock);

bool cmp_sem_priority (const struct list_elem *a,
           const struct list_elem *b);
bool isEmpty(struct semaphore_elem * a);






#endif


