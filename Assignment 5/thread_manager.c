#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "ThreadData.h"
#include "InputNode.h"

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating ThreadData
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER; 
//thread mutex lock for critical section of creating InputNode
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical section of setting is_reading_complete to true
pthread_mutex_t tlock4 = PTHREAD_MUTEX_INITIALIZER;

// Condition for waking up thread 2
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void* thread_runner(void*);
pthread_t thread1, thread2;
ThreadData* p = NULL;

// Variables for reading input from stdin
char buffer[100];
char* input;

// Variables for the linked list
InputNode* currNode = NULL;
InputNode* head = NULL;
bool didHeadChange = false;

// Variable for indexing of messages by the logging function
int logindex = 0;

// A flag to indicate if the reading of input is complete,
// so the other thread knows when to stop
bool is_reading_complete = false;

// Variables to store date and time components
int hours, minutes, seconds, day, month, year;
time_t now;
struct tm *local;


/*********************************************************
MAIN FUNCTION
*********************************************************/
int main() {
    // Obtain current time
    time(&now);

    // localtime converts a time_t value to calendar time
    local = localtime(&now);

    // Initialize day, month, and year
    day = local->tm_mday;            // get day of month (1 to 31)
    month = local->tm_mon + 1;       // get month of year (0 to 11)
    year = local->tm_year + 1900;    // get year since 1900

    printf("create first thread\n");
    pthread_create(&thread1, NULL,thread_runner, NULL);
    
    printf("create second thread\n");
    pthread_create(&thread2, NULL, thread_runner, NULL);
    
    printf("wait for first thread to exit\n");
    pthread_join(thread1, NULL);
    printf("first thread exited\n");

    printf("wait for second thread to exit\n");
    pthread_join(thread2, NULL);
    printf("second thread exited\n");

    exit(0);
}


/**********************************************************************
RUNNER FUNCTION FOR EACH THREAD
**********************************************************************/
void* thread_runner(void* x) {
    pthread_t me = pthread_self();

    // CRITICAL SECTION: Allocating p
    pthread_mutex_lock(&tlock2);
    if (p == NULL) {
        p = (ThreadData *) malloc(sizeof(ThreadData));
        p->creator = me;
    }

    printf("This is thread %ld (p=%p)\n", me, p);
    pthread_mutex_unlock(&tlock2);

    // THREAD 1: Read stdin and create linkedlist
    if (p != NULL && p->creator == me) {
        // Read each line from STDIN
        while ((input = fgets(buffer, 100, stdin)) != NULL) {
            if (*input == '\n') break;


            // CRITICAL SECTION: Creating linkedlist
            pthread_mutex_lock(&tlock3);
            currNode = (InputNode *) malloc(sizeof(InputNode));
            CreateInputNode(currNode, input, head);
            head = currNode;
            didHeadChange = true;
            pthread_mutex_unlock(&tlock3);


            // CRITICAL SECTION: Print that a new node is in the linkedlist
            pthread_mutex_lock(&tlock1);
            hours = local->tm_hour;         // get hours since midnight (0-23)
            minutes = local->tm_min;        // get minutes passed after the hour (0-59)
            seconds = local->tm_sec;        // get seconds passed after minute (0-59)

            // Before midday
            if (hours < 12)
                printf("Logindex %d, thread %ld, PID %d, %02d/%02d/%d %02d:%02d:%02d am:  Head of linked list contains %s", logindex, me, getpid(), day, month, year, hours, minutes, seconds, input);

            // After midday
            else
                printf("Logindex %d, thread %ld, PID %d, %02d/%02d/%d %02d:%02d:%02d pm:  Head of linked list contains %s", logindex, me, getpid(), day, month, year, hours - 12, minutes, seconds, input);

            ++logindex;
            pthread_cond_signal(&condition);
            pthread_mutex_unlock(&tlock1);
        }

        // CRITICAL SECTION: Set is_reading_compleete and didHeadChange to true
        pthread_mutex_lock(&tlock4);
        is_reading_complete = true;
        didHeadChange = true;
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&tlock4);
    }

    // THREAD 2: Print head of linkedlist
    else {
        while (!is_reading_complete) {
            // CRITICAL SECTION: Wait for head to change if it hasn't
            pthread_mutex_lock(&tlock3);
            while (!didHeadChange)
                pthread_cond_wait(&condition, &tlock3);

            didHeadChange = false;
            pthread_mutex_unlock(&tlock3);


            // CRITICAL SECTION: Print the head content 
            pthread_mutex_lock(&tlock1);
            // Thread 2 could be waiting for the head to change, but then the user hits enter, which causes
            // thread 2 to stop waiting and the linked list to be deallocated.
            // The if statement prevents the program from continuing into this block of code and giving a seg fault
            // when accessing the linked list since the linked list could be deallocated.
            if (!is_reading_complete) {
                hours = local->tm_hour;         // get hours since midnight (0-23)
                minutes = local->tm_min;        // get minutes passed after the hour (0-59)
                seconds = local->tm_sec;        // get seconds passed after minute (0-59)

                // Before midday
                if (hours < 12)
                    printf("Logindex %d, thread %ld, PID %d, %02d/%02d/%d %02d:%02d:%02d am:  Head of linked list contains %s", logindex, me, getpid(), day, month, year, hours, minutes, seconds, head->input);

                // After midday
                else
                    printf("Logindex %d, thread %ld, PID %d, %02d/%02d/%d %02d:%02d:%02d pm:  Head of linked list contains %s", logindex, me, getpid(), day, month, year, hours - 12, minutes, seconds, head->input);

                ++logindex;
            }
            pthread_mutex_unlock(&tlock1);
        }
    }

    // CRITICAL SECTION: Free linked list
    pthread_mutex_lock(&tlock3);
    if(head != NULL) {
        FreeNodes(head);
        head = NULL;

        // CRITICAL SECTION: Print that the linkedlist is freed
        pthread_mutex_lock(&tlock1);
        printf("LogIndex %d, Thread %ld, PID %d: freed linked list.\n", logindex, me, getpid());
        ++logindex;
        pthread_mutex_unlock(&tlock1);
    }
    pthread_mutex_unlock(&tlock3);

    // CRITICAL SECTION: Free p
    pthread_mutex_lock(&tlock2);
    if (p != NULL && p->creator == me) {
        printf("LogIndex %d, This is thread %ld and I didn't touch ThreadData\n", logindex, me);
        ++logindex;
    }
    else {
        free(p);
        p = NULL;

        // CRITICAL SECTION: Print that the ThreadData is freed
        pthread_mutex_lock(&tlock1);
        printf("LogIndex %d, This is thread %ld and I deleted the ThreadData object\n", logindex, me);
        ++logindex;
        pthread_mutex_unlock(&tlock1);
    }
    pthread_mutex_unlock(&tlock2);

    return NULL;
}