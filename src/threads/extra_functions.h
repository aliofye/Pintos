#ifndef THREADS_extra_functions_H
#define THREADS_extra_functions_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h" //HB added 


bool
thread_higher_priority (const struct list_elem *a_,
                        const struct list_elem *b_,
                         void *aux UNUSED);

bool
thread_lower_priority (const struct list_elem *a_,
                        const struct list_elem *b_,
                         void *aux UNUSED);

void thread_yield_to_higher_priority (void);

bool
thread_donor_priority(const struct list_elem *a_,
                        const struct list_elem *b_,
                          void *aux UNUSED);

void recompute_thread_priority (struct thread*);

void sort_ready_list(void);




#endif


