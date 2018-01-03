//NAME: Chungchhay Kuoch
//EMAIL: chungchhay@ucla.edu
//ID: 004 843 308

#include "SortedList.h"
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

int lock = 0; //Locked 1, unlocked 0
pthread_mutex_t threadLock; //similiar to lock but this one is for pthread mutex
char syncOption;
int opt_yield = 0; //Option to test yield
long iteration = 1; //number of iteration
long thread = 1; //number of thread
long list = 1; //number of list
long *mutexTimes; //acquire mutex time
long *spinLockTimes; 
int listElements;
SortedListElement_t **allElements;
SortedList_t **subList; // sublist
SortedListElement_t *currentElement;
int exitStatus = 0; //0 exits normally

void acquire_lock(int *spinLock);
void release_spinlock(int *spinLock);
void insertList(int tid);
void lookUpNDeleteElement(int tid);
void listLength(void);
void *executionFunc(void *arg);
char *generateRandomKeysForListElement(void);
unsigned int getHashNumber(const char *str);

int main(int argc, char **argv)
{
  int c = 0; //argument option
  struct timespec start, end;
  int i; //just for counting and loop
  int index;
    
    static struct option longOptions[] =
      {
        {"threads", required_argument, 0, 't'},
        {"iterations", required_argument, 0, 'i'},
        {"sync", required_argument, 0, 's'},
        {"yield", required_argument, 0, 'y'},
        {"lists", required_argument, 0, 'l'},
	{0, 0, 0, 0}
      };
    
    while (1)
      {
        c = getopt_long(argc, argv, "t:i:s:y:l:", longOptions, &index);
        
        if (c == -1)
	  {
            break;
	  }
        
        switch(c)
	  {
	  case 't':
		thread = atoi(optarg);
	    break;
	  case 'i':
		iteration = atoi(optarg);
	    break;
	  case 's':
	    syncOption = *optarg;
                
	    if (syncOption == 'm')
	      {
		pthread_mutex_init(&threadLock, NULL);
	      }
	    break;
	  case 'y':
	    i = 0;
	    unsigned long len = strlen(optarg);
	    if (len > 3)
	      {
		fprintf(stderr, "Error: wrong argument for --yield: %c\n", optarg[i]);
		exit(1);
	      }
                
	    while (optarg[i] != '\0')
	      {
		if (optarg[i] == 'i')
		  {
		    opt_yield |= INSERT_YIELD;
		  }
		else if (optarg[i] == 'd')
		  {
		    opt_yield |= DELETE_YIELD;
		  }
		else if (optarg[i] == 'l')
		  {
		    opt_yield |= LOOKUP_YIELD;
		  }
		else {
		  fprintf(stderr, "Error: wrong argument for --yield: %c\n", optarg[i]);
		  exit(1);
		}
		i++;
	      }
	    break;
	  case 'l':
		list = atoi(optarg);
	    break;
	  default:
	    //Input other arguments than those 5
	    fprintf(stderr, "Arguments Error");
	    exit(1);
	  }
      }
    
    listElements = thread * iteration;
    allElements = malloc(listElements *sizeof(SortedListElement_t*));
    
    for (i = 0; i < listElements; i++)
      {
        allElements[i] = malloc(sizeof(SortedListElement_t));
        allElements[i]->prev = NULL;
        allElements[i]->next = NULL;
        allElements[i]->key = generateRandomKeysForListElement();
      }
    
    pthread_t *threads = malloc(thread * sizeof(pthread_t));
    int *tid = malloc(thread * sizeof(int));
    
    subList = malloc(list * sizeof(SortedList_t*));
    for (i = 0; i < list; i++)
      {
        subList[i] = malloc(sizeof(SortedList_t));
        subList[i]->prev = subList[i];
        subList[i]->next = subList[i];
        subList[i]->key = NULL;
      }
    
    mutexTimes = malloc(thread * sizeof(long));
    spinLockTimes = malloc(thread * sizeof(long));
    //Starting timing
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
      {
        perror("Error to start the time");
        exitStatus = 1;
      }
    
    for (i = 0; i < thread; i++)
      {
        tid[i] = i;
        int temp = pthread_create(&threads[i], NULL, executionFunc, (void *) &tid[i]);
        if (temp)
	  {
            perror("Creating thread(s) error");
            exitStatus = 1;
	  }
      }
    
    for (i = 0; i < thread; i++)
      {
        int temp = pthread_join(threads[i], NULL);
        if (temp)
	  {
            perror("Joining thread(s) error");
            exitStatus = 1;
	  }
      }
    
    //Ending time
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
      {
        perror("Error to end the time");
        exitStatus = 1;
      }
    
    for (i = 0; i < list; i++)
      {
        if (SortedList_length(subList[i]) != 0)
	  {
            perror("Error: List is not empty after inserting and deleting");
            exit(1);
	  }
      }
    
    long totalOperators = iteration * thread * 3;
    //Want to calculate time in nanosecond, therefore sec to nanosecond is multiplied by 1Billion
    long threadRunTime = ((long) end.tv_sec - (long) start.tv_sec) * 1000000000 + ((long) end.tv_nsec - (long) start.tv_nsec);
    long eachOptRuntime = threadRunTime / totalOperators;
    
    printf("list-");
    
    if (opt_yield & INSERT_YIELD)
      {
        printf("i");
      }
    
    if (opt_yield & DELETE_YIELD)
      {
        printf("d");
      }
    
    if (opt_yield & LOOKUP_YIELD)
      {
        printf("l");
      }
    
    if (opt_yield == 0)
      {
        printf("none");
      }
    
    //Sync options
    printf("-");
    if (syncOption == '\0')
      {
        printf("none");
      }
    else
      {
        printf("%c", syncOption);
      }
    
    printf(",");
    
    printf("%ld,%ld,%ld,%ld,%ld,%ld", thread, iteration, list, totalOperators, threadRunTime, eachOptRuntime);
    
    if (syncOption == 'm') {
      long actualMutexTimeTotal = 0;
      // get the total time that the threads waited to acquire the mutex
      for (i = 0; i < thread; i++) {
	actualMutexTimeTotal += mutexTimes[i];
      }

      long averageMutexWait = actualMutexTimeTotal / (thread);
      printf(",%ld", averageMutexWait);
    }
    else if (syncOption == 's') 
      {
      long actualSpinLockTimeTotal = 0;
      for (i = 0; i < thread; i++) {
	actualSpinLockTimeTotal += spinLockTimes[i];
      }
      long averageSpinLockWait = actualSpinLockTimeTotal / (thread);
      printf(",%ld", averageSpinLockWait);
    }
    else 
      {
	printf(",0");
      }

    printf("\n");
    
    exit(exitStatus);
}

//Lock for spin lock
void acquire_lock(int *spinLock)
{
  while(__sync_lock_test_and_set(spinLock, 1));
}

//unlock for spin lock
void release_spinlock(int *spinLock)
{
  __sync_lock_release(spinLock);
}

void insertList(int tid)
{
  int i;
  for (i = tid; i < listElements; i += thread)
    {
      const char *temp = allElements[i]->key;
      unsigned int subListTemp = getHashNumber(temp) % list;
      SortedList_insert(subList[subListTemp], allElements[i]);
    }
}

//Helper function for looking up and deleting element in the list
void lookUpNDeleteElement(int tid)
{
  int i;
  for (i = tid; i < listElements; i += thread)
    {
      const char *temp = allElements[i]->key;
      unsigned int subListTemp = getHashNumber(temp) % list;
      currentElement = SortedList_lookup(subList[subListTemp], allElements[i]->key);
      if (currentElement == NULL)
        {
	  perror("Error: cannot find the element");
	  exit(1);
        }
        
      if (SortedList_delete(currentElement) != 0)
        {
	  perror("Error: cannot delete the element");
	  exit(1);
        }
    }
}

void listLength()
{
  int i;
  for (i = 0; i < list; i++)
    {
      if (SortedList_length(subList[i]) == -1)
        {
	  perror("Error: List length");
	  exit(1);
        }
      else
        {
	  SortedList_length(subList[i]);
        }
    }
}

//Main part of this program is to do the execution of the list
void *executionFunc(void *arg)
{
  int tid = * (int *) arg;
    
  //3 sync options, if sync is m it is mutex lock. If sync is s,
  //it is spin lock. Otherwise, no lock.
  if (syncOption == 'm')
    {
      struct timespec MutexStartTimes;
      struct timespec MutexEndTimes;
      if (clock_gettime(CLOCK_MONOTONIC, &MutexStartTimes))
        {
	  perror("Error Mutex start time");
	  exitStatus = 1;
        }
        
      pthread_mutex_lock(&threadLock);
        
      if (clock_gettime(CLOCK_MONOTONIC, &MutexEndTimes))
        {
	  perror("Error Mutex end time");
	  exitStatus = 1;
        }
        
      long actualMutexTimes = ((long) MutexEndTimes.tv_sec - (long) MutexStartTimes.tv_sec) * 1000000000 + ((long)MutexEndTimes.tv_nsec - (long)MutexStartTimes.tv_nsec);
      mutexTimes[tid] = actualMutexTimes;
    }
    
  if (syncOption == 's')
    {
      struct timespec spinLockStartTimes;
      struct timespec spinLockEndTimes;

      if (clock_gettime(CLOCK_MONOTONIC, &spinLockStartTimes))
        {
          perror("Error spin lock start time");
          exitStatus = 1;
        }

      acquire_lock(&lock);

      if (clock_gettime(CLOCK_MONOTONIC, &spinLockEndTimes))
        {
          perror("Error spin lock end time");
          exitStatus = 1;
        }

      long actualSpinLocktimes = ((long) spinLockEndTimes.tv_sec - (long) spinLockStartTimes.tv_sec) * 1000000000 + ((long) spinLockEndTimes.tv_nsec - (long) spinLockStartTimes.tv_nsec);

      spinLockTimes[tid] = actualSpinLocktimes;
    }
    
  //Performing the tasks
  insertList(tid);
  listLength();
  lookUpNDeleteElement(tid);
    
  //Release if there is any lock
  if (syncOption == 'm')
    {
      pthread_mutex_unlock(&threadLock);
    }
    
  if (syncOption == 's')
    {
      release_spinlock(&lock);
    }
    
  pthread_exit(NULL);
}

//Get a random key for each element in the list
char *generateRandomKeysForListElement(void)
{
  char *string = "abcdefghijkmlnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  char *key = malloc(5 * sizeof(char)); //let's say each key is 5 bytes long including \0
    
  int i;
  for (i = 0; i < 4; i++)
    {
      key[i] = string[rand() % strlen(string)];
    }
    
  key[4] = '\0';
  return key;
}

unsigned int getHashNumber(const char *str)
{
  unsigned int hash = 5381;
  int c;
    
  while ((c = *str++))
    {
      hash = ((hash << 5) + hash) + c;
    }
    
  return hash;
}

