#ifndef THREADS_extra_functions_H
#define THREADS_extra_functions_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h" //HB added 

//extern bool listCalls;



bool greater_than_31 (const struct list_elem*,
                        const struct list_elem *);

bool less_than_31 (const struct list_elem *,const struct list_elem *);

void let_higher_go_first (void);

bool donorUpdate(const struct list_elem *,const struct list_elem *);

void newPriority (struct thread*);

void callListSort(void);

bool makeDecisions(const struct list_elem *a, const struct list_elem *b, bool isDonationElem, bool isGreater);

void iterateThrough(struct list_elem * element, struct lock * lock);

bool semaphoreWaiter (const struct list_elem *a,const struct list_elem *b);

bool isEmpty(struct semaphore_elem * a);






#endif


