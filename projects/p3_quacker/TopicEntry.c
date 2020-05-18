/*
 * Seth Tal
 * 05/18/2020
 * Project 3 - Quacker...
*/


#define URLSIZE 512
#define CAPSIZE 1024
#define MAXENTRIES 3


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


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
    TopicEntry *head;
    TopicEntry *tail;
    TopicEntry entries[MAXENTRIES];

} EntryQueue;


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

void EntryEnqueue(EntryQueue *queue, TopicEntry entry)
{
    if (isFull(queue))
    {
        fprintf(stdout, "Queue of Entries is full. Entry was not enqueue'd.\n");
        return;
    }

    queue->back = (queue->back + 1)%queue->capacity;
    queue->entries[queue->back] = entry;
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
