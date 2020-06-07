/*
 * Seth Tal
 * 05/18/2020
 * Project 3 - Quacker...
*/


#define URLSIZE 512
#define CAPSIZE 1024
#define MAXENTRIES 100
#define MAXNUMBUFFERS 10
#define MAXPUBSUB 50
#define NUMPROXIES 10


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>


/* ------------------------------------------- */
/*                  Struct Defs                */
/* ------------------------------------------- */

// A Topic Entry
typedef struct TopicEntry
{
    // Required Members for the project
    int entryNum;
    struct timeval timeStamp;
    int pubID;
    char photoURL[URLSIZE];
    char photoCaption[CAPSIZE];

} TopicEntry;

// A Topic Entry Queue
typedef struct EntryQueue
{
    int size;
    int front;
    int back;
    int capacity;
    int lastEntries[MAXNUMBUFFERS];
    TopicEntry *head;
    TopicEntry *tail;
    TopicEntry entries[MAXENTRIES];

} EntryQueue;

// So far these two structs are unused
// struct PublisherPoxies
// {
//     int flag;       // is proxy free? 1: no, 0: yes
//     int id;         // this thread's ID
//     pthread_t pub;  // the publisher thread for this proxy
// };

// struct SubscriberPoxies
// {
//     int flag;       // is proxy free? 1: no, 0: yes
//     int id;         // this thread's ID
//     pthread_t sub;  // the subscriber thread for this proxy
// };

struct pubargs
{
    int id;
    int flag;
    TopicEntry entries[MAXENTRIES];
};

struct subargs
{
    int id;                   // thread id
    int flag;                 // thread status
    TopicEntry tempEntry;     // temp Topic Entry
};


/* ------------------------------------------- */
/*                   Queue Methods             */
/* ------------------------------------------- */

// Init Queue
EntryQueue *InitializeEntryQueue()
{
    EntryQueue *result = (EntryQueue *)malloc(sizeof(EntryQueue));

    result->capacity = MAXENTRIES;
    result->size = 0;
    result->front = 0;
    result->back = result->capacity - 1;
    result->head = NULL;
    result->tail = NULL;

    return result;
}

// Free queue
void FreeEntryQueue(EntryQueue *queue)
{
    free(queue);
}

// Check if Queue is full
int isFull(EntryQueue *queue)
{
    return (queue->size == queue->capacity);
}

// Check if Queue is empty
int isEmpty(EntryQueue *queue)
{
    return (queue->size == 0);
}

void EntryEnqueue(EntryQueue *queue, TopicEntry *entry)
{
    if (isFull(queue))
    {
        fprintf(stdout, "Queue of Entries is full. Entry was not enqueue'd.\n");
        return;
    }

    // Set new value for last index of entry array
    queue->back = (queue->back + 1)%queue->capacity;

    // enqueue new entry
    queue->entries[queue->back] = *entry;

    // Set timeStamp for current entry
    gettimeofday(&entry->timeStamp, NULL);
    // fprintf(stdout, "TimeStamp: %ld\n", entry.timeStamp.tv_sec);

    // increment size of queue
    queue->size++;

    queue->head = &queue->entries[queue->front];
    queue->tail = &queue->entries[queue->back];
}

TopicEntry EntryDequeue(EntryQueue *queue)
{
    if (isEmpty(queue))
    {
        fprintf(stderr, "ERROR. Queue of Entries is empty.\n");
        exit(EXIT_FAILURE);
    }

    TopicEntry result = queue->entries[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size--;

    queue->head = &queue->entries[queue->front];
    queue->tail = &queue->entries[queue->back];

    return result;
}

int GetEntry(EntryQueue *queue, int lastEntry, TopicEntry *topic)
{
    // Case 1
    if (isEmpty(queue))
        return 0;
    
    // Case 2
    for (int i = 0; i < queue->size; i++)
    {
        if (queue->entries[queue->front + i].entryNum == (lastEntry + 1))
        {
            topic->entryNum = queue->entries[queue->front + i].entryNum;
            // ! printf("Last Entry+1 = %d. Entry Num: %d vs. %d\n", lastEntry + 1, topic->entryNum, queue->entries[queue->front + i].entryNum);
            topic->timeStamp = queue->entries[queue->front + i].timeStamp;
            return 1;
        }
    }

    // Case 3
    if (queue->head->entryNum < (lastEntry+1) && queue->tail->entryNum < (lastEntry+1))
        return 0;

    // Case 4
    for (int i = 0; i < queue->size; i++)
    {
        if (queue->entries[queue->front + i].entryNum > (lastEntry + 1))
        {
            topic->entryNum = queue->entries[queue->front + i].entryNum;
            topic->timeStamp = queue->entries[queue->front + i].timeStamp;
            return topic->entryNum;
        }
    }

    return 0;

}
