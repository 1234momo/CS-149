#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ThreadData.h"
#include "InputNode.h"

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating ThreadData
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER; 
//thread mutex lock for critical section of creating InputNode
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER; 


void* thread_runner(void*);
pthread_t thread1, thread2;

// Variables for reading input from stdin
char buffer[100];
char* input;

InputNode currNode = NULL;
InputNode head = NULL;
didHeadChange = false;

ThreadData* p = NULL;


// Variable for indexing of messages by the logging function
int logindex = 0;
int *logip = &logindex;


//A flag to indicate if the reading of input is complete, 
//so the other thread knows when to stop
bool is_reading_complete = false;

// variables to store date and time components
int hours, minutes, seconds, day, month, year;

// time_t is arithmetic time type
time_t now;

// Obtain current time
// time() returns the current time of the system as a time_t value
time(&now);

// localtime converts a time_t value to calendar time and 
// returns a pointer to a tm structure with its members 
// filled with the corresponding values
struct tm *local = localtime(&now);


/*********************************************************
// function main  ------------------------------------------------- 
*********************************************************/
int main() {
    printf("create first thread");
    pthread_create(&thread1, NULL,thread_runner, NULL);
    
    printf("create second thread");
    pthread_create(&thread2, NULL, thread_runner, NULL);
    
    printf("wait for first thread to exit");
    pthread_join(thread1, NULL);
    printf("first thread exited");

    printf("wait for second thread to exit");
    pthread_join(thread2, NULL);
    printf("second thread exited");

    exit(0);
}


/**********************************************************************
// function thread_runner runs inside each thread -------------------------------------------------- 
**********************************************************************/
void* thread_runner(void* x) {
    pthread_t me = pthread_self();
    printf("This is thread %ld (p=%p)", me, p);
    
    // CRITICAL SECTION: Allocating p
    pthread_mutex_lock(&tlock2); 
    if (p == NULL) {
        p = (ThreadData*) malloc(sizeof(ThreadData));
        p->creator = me;
    }
    pthread_mutex_unlock(&tlock2);

    // CRITICAL SECTION: Thread 1 and Thread 2 begins
    pthread_mutex_lock(&tlock2); 

    // THREAD 1: Read stdin and create linkedlist
    if (p != NULL && p->creator == me) {

        // Read each line from STDIN
        while ((input = fgets(buffer, 100, stdin)) != -1) {
            if (input == '\n') break;

            // CRITICAL SECTION: Creating linkedlist
            pthread_mutex_lock(&tlock3); 

            currNode = (InputNode*) malloc(sizeof(InputNode));
            CreateInputNode(currNode, input, head);
            head = currNode;
            didHeadChange = true;

            pthread_mutex_unlock(&tlock3); 


            // CRITICAL SECTION: Print that a new node is in the linkedlist
			pthread_mutex_lock(&tlock1);
            hours = local->tm_hour;      	// get hours since midnight (0-23)
            minutes = local->tm_min;     	// get minutes passed after the hour (0-59)
            seconds = local->tm_sec;     	// get seconds passed after minute (0-59)

            day = local->tm_mday;        	// get day of month (1 to 31)
            month = local->tm_mon + 1;   	// get month of year (0 to 11)
            year = local->tm_year + 1900;	// get year since 1900

            // before midday
            if (hours < 12)	
                printf("Logindex %d, thread 1, PID %d, %02d/%02d/%d %02d:%02d:%02d am:  Head of linked list contains line %s", logindex, getpid(), day, month, year, hours, minutes, seconds, input);

            // after midday
            else	
                printf("Logindex %d, thread 1, PID %d, %02d/%02d/%d %02d:%02d:%02d pm:  Head of linked list contains line %s", logindex, getpid(), day, month, year, hours - 12, minutes, seconds, input);
            
            ++logindex;
            pthread_cond_signal(&listCondition);
            pthread_mutex_unlock(&tlock1); 
        }

        is_reading_complete = true;
    }

    // THREAD 2: print head of linkedlist and frees memory
    else {
        /**
         * TODO Free the THREADATA object. Freeing should be done by the other thread from the one that created it.
         * See how the ThreadData was created for an example of how this is done.
         */
        
        while (!is_reading_complete) {

            // CRITICAL SECTION: Wait for head to change if it hasn't
            pthread_mutex_lock(&tlock3); 
            while (!didHeadChange)
                pthread_cond_wait(&listCondition, &tlock3);
        
            didHeadChange = false;
            pthread_mutex_unlock(&tlock3); 

            // CRITICAL SECTION: Print the head content 
            pthread_mutex_lock(&tlock1);
            hours = local->tm_hour;      	// get hours since midnight (0-23)
            minutes = local->tm_min;     	// get minutes passed after the hour (0-59)
            seconds = local->tm_sec;     	// get seconds passed after minute (0-59)

            day = local->tm_mday;        	// get day of month (1 to 31)
            month = local->tm_mon + 1;   	// get month of year (0 to 11)
            year = local->tm_year + 1900;	// get year since 1900

            // before midday
            if (hours < 12)	
                printf("Logindex %d, thread 1, PID %d, %02d/%02d/%d %02d:%02d:%02d am:  Head of linked list contains line %s", logindex, getpid(), day, month, year, hours, minutes, seconds, head -> input);

            // after midday
            else	
                printf("Logindex %d, thread 1, PID %d, %02d/%02d/%d %02d:%02d:%02d pm:  Head of linked list contains line %s", logindex, getpid(), day, month, year, hours - 12, minutes, seconds, head -> input);
            
            ++logindex;
            pthread_mutex_unlock(&tlock1); 
        }
    }
    pthread_mutex_unlock(&tlock2);

    // CRITICAL SECTION: free p
    pthread_mutex_lock(&tlock2);
    if (p != NULL && p->creator != me) {
        free(p);
        p = NULL;
        pthread_mutex_lock(&tlock1);
		printf("This is thread %ld and I deleted the THREAD_DATA object\n", me);
		pthread_mutex_unlock(&tlock1);
    }
    pthread_mutex_unlock(&tlock2);
    
    // CRITICAL SECTION: free linked list	
	pthread_mutex_lock(&tlock3);
	if(head != NULL) {
		FreeNodes(head);
		head = NULL;

		pthread_mutex_lock(&tlock1);
		printf("LogIndex %d, Thread %ld, PID %d: freed linked list.\n", logIndex, currThread, getpid());
        ++logIndex;
		pthread_mutex_unlock(&tlock1);
	}
	pthread_mutex_unlock(&tlock3);	

    pthread_exit(NULL);
    return NULL;
}