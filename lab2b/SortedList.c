//NAME: Chungchhay Kuoch                                                               
//EMAIL: chungchhay@ucla.com                                                      //ID: 004 843 308 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>
#include "SortedList.h"

int opt_yield;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  if (opt_yield & INSERT_YIELD) {
    sched_yield();
  }
    
  SortedList_t *head = list;
  SortedList_t *current = list->next;
    
  while (current != head) {
    if (strcmp(element->key, current->key) <= 0) {
      break;
    }
    current = current->next;
  }
    
  element->prev = current->prev;
  element->next = current;
  current->prev->next = element;
  current->prev = element;
}

int SortedList_delete( SortedListElement_t *element)
{
  if (opt_yield & DELETE_YIELD) {
    sched_yield();
  }
    
  if ((element->prev->next != element) || (element->next->prev != element))
    return 1;
    
  element->prev->next = element->next;
  element->next->prev = element->prev;
  free(element);
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) 
{
  if (opt_yield & LOOKUP_YIELD) {
    sched_yield();
  }
    
  SortedListElement_t *current = list->next;
    
  while (current != list) {
    if (strcmp(current->key, key) == 0)
      return current;
        
    current = current->next;
  }
  return NULL;   // couldn't find key
}

int SortedList_length(SortedList_t *list)
{
  if (opt_yield & LOOKUP_YIELD) {
    sched_yield();
  }
    
  int length = 0;
  SortedListElement_t *current = list->next;
    
  while (current != list) {
    if ((current->prev->next != current) || (current->next->prev != current)) {
      return -1;
    }
    ++length;
    current = current->next;
  }
  return length;
}
