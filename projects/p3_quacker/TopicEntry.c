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
    int topicID;      // topic ID
    char topicName[BUFSIZ];  // topic name
    int capacity;     // topic length

    int size;         // current number of entries
    int front;        // newest entry
    int back;         // oldest entry
    int lastEntries[MAXNUMBUFFERS];
    TopicEntry entries[MAXENTRIES];

} EntryQueue;

struct pubargs
{
    int id;
    int flag;
    char filename[BUFSIZ];
    TopicEntry entries[MAXENTRIES];
};

struct subargs
{
    int id;                   // thread id
    int flag;                 // thread status
    char filename[BUFSIZ];    // filename to pull commands from
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

    // Set timeStamp for current entry
    gettimeofday(&entry->timeStamp, NULL);

    // enqueue new entry
    queue->entries[queue->back] = *entry;
    // fprintf(stdout, "TimeStamp: %ld\n", entry->timeStamp.tv_sec);

    // increment size of queue
    queue->size++;
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
            topic->timeStamp = queue->entries[queue->front + i].timeStamp;
            strcpy(topic->photoURL, queue->entries[queue->front + i].photoURL);
            strcpy(topic->photoCaption, queue->entries[queue->front + i].photoCaption);
            return 1;
        }
    }

    // Case 3
    if (queue->entries[queue->front].entryNum < (lastEntry+1) && queue->entries[queue->back].entryNum < (lastEntry+1))
        return 0;

    // Case 4
    for (int i = 0; i < queue->size; i++)
    {
        if (queue->entries[queue->front + i].entryNum > (lastEntry + 1))
        {
            topic->entryNum = queue->entries[queue->front + i].entryNum;
            topic->timeStamp = queue->entries[queue->front + i].timeStamp;
            strcpy(topic->photoURL, queue->entries[queue->front + i].photoURL);
            strcpy(topic->photoCaption, queue->entries[queue->front + i].photoCaption);
            return topic->entryNum;
        }
    }

    return 0;

}
