#ifndef THREADS_extra_functions_H
#define THREADS_extra_functions_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h" //HB added 
//#include "threads/thread.h" //this was causing errors

//new file, the .h isn't  added in makefile b/c it's unnecessary 




bool doesFirstThreadHaveHigherPriority 
	(const struct list_elem *a,const struct list_elem *b); //HB MADE CHANGE

bool doesFirstThreadHaveLowerPriority   
	(const struct list_elem *a,const struct list_elem *b); //HB MADE CHANGE

void let_higher_go_first (void); //HB MADE CHANGE

bool donorUpdate(const struct list_elem *a,const struct list_elem *b); //HB MADE CHANGE

bool makeDecisions
(const struct list_elem *a,const struct list_elem *b, bool shouldRightbeHigher, bool isjustelem); //HB MADE CHANGE

void newPriority (struct thread* t , void * ls UNUSED); //HB MADE CHANGE

void callListSort(void); //HB MADE CHANGE

/*void lockReleaseHelper(struct lock * lock);*/






#endif