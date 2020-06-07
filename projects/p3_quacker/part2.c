/*
 * Seth Tal
 * 05/18/2020
 * Project 3 - Quacker...
*/


// Includes
#include "TopicEntry.c"


// Function declarations
void init(int len);
int enqueue_routine(int buffid, TopicEntry *entry);
int dequeue_routine(int buffid, TopicEntry *entry);
void *publisher(void *args);
void *subscriber(void *args);
void *cleanup_thread(void *args);


// Global data
EntryQueue buffers[MAXNUMBUFFERS];  // list of all Entry Queues
struct pubargs pubARGs[MAXPUBSUB];              // 
struct subargs subARGs[MAXPUBSUB];              // 
pthread_t pubs[NUMPROXIES];          // Thread ID for pubs
pthread_t subs[NUMPROXIES];          // Thread ID for subs
pthread_attr_t attr;                // Thread Attributes

pthread_mutex_t mutexes[MAXNUMBUFFERS];

int numBufs;
int numEntries;

pthread_t cleanupID;
struct pubargs cleanupARGs;

const double deltaTime = 1.0;


void usage(int argc)
{
    if (argc != 5)
    {
        fprintf(stderr, "USAGE: ./server <INT> <INT> <INT> <INT>\n");
        exit(EXIT_FAILURE);
    }
}

// Main
int main(int argc, char *argv[])
{
    usage(argc);

    int timeToSleep, pubCount, subCount;

    timeToSleep = atoi(argv[1]);
    pubCount = atoi(argv[2]);
    subCount = atoi(argv[3]);
    numBufs = atoi(argv[4]);

    fprintf(stdout, "\n%d, %d, %d, %d\n\n", timeToSleep, pubCount, subCount, numBufs);

    printf("\n");
    printf("Running program...\n\n");
    
    numEntries = 5;
    init(numEntries);

    // spawn publisher threads
    for (int i = 0; i < pubCount; i++)
    {
        pubARGs[i].id = i;
        pubARGs[i].flag = 1;
        pthread_create(&pubs[i], &attr, publisher, (void *) &pubARGs[i]);
    }

    // spawn subscriber threads
    for (int i = 0; i < subCount; i++)
    {
        subARGs[i].id = i;
        subARGs[i].flag = 1;
        pthread_create(&subs[i], &attr, subscriber, (void *) &subARGs[i]);
    }

    cleanupARGs.id = 0;
    pthread_create(&cleanupID, &attr, cleanup_thread, (void *) &cleanupARGs);

    // Join threads
    for (int i = 0; i < NUMPROXIES; i++)
    {
        pthread_join(pubs[i], NULL);
        pthread_join(subs[i], NULL);
    }

    // call main proc to sleep
    // sleep(timeToSleep);

    // for (int i = 0; i < numBufs; i++)
    // {
    //     free(&buffers[i]);
    // }
    
    printf("\nDone.\n");
    return EXIT_SUCCESS;
}

void init(int len)
{
    // Generate entry queues
    for (int i = 0; i < MAXNUMBUFFERS; i++)
    {
        buffers[i].capacity = len;
        buffers[i].size = 0;
        buffers[i].front = 0;
        buffers[i].back = buffers[i].capacity - 1;
        buffers[i].head = NULL;
        buffers[i].tail = NULL;

        for (int j = 0; j < numBufs; j++)
            buffers[i].lastEntries[j] = 0;
    }

    // generate buffer semaphores
    for (int i = 0; i < MAXNUMBUFFERS; i++)
    {
        pthread_mutex_init(&(mutexes[i]), NULL);
    }

    pthread_attr_init(&attr);
}

int enqueue_routine(int buffid, TopicEntry *entry)
{
    if (isFull(&buffers[buffid]))
        return -1;
    
    EntryEnqueue(&buffers[buffid], entry);

    return 0;
}

int dequeue_routine(int buffid, TopicEntry *entry)
{
    if (isEmpty(&buffers[buffid]))
        return -1;

    *entry = EntryDequeue(&buffers[buffid]);

    return 0;
}

void *publisher(void *args)
{
    int bufferID;    // buffer id
    int threadID;    // thread id
    // int threadState; // thread state

    threadID = ((struct pubargs *) args)->id;
    // threadState = ((struct pubargs *) args)->state;

    int entryNumber = 1;

    fprintf(stdout, "Publisher(%d), %ld\n", threadID, pthread_self());
    while (1)
    {
        TopicEntry testEntry;
        testEntry.entryNum = entryNumber;

        for (bufferID = 0; bufferID < numBufs; bufferID++)
        {
            pthread_mutex_lock(&mutexes[bufferID]);
            
            if (enqueue_routine(bufferID, &testEntry) == -1)
            {
                fprintf(stdout, "Publisher(%d), Buffer[%d] is full.\n", threadID, bufferID);
                pthread_mutex_unlock(&(mutexes[bufferID]));
                sleep(1);
            }
            else
            {
                fprintf(stdout, "Publisher(%d), Entry(%d) pushed onto Buffer[%d]\n", threadID, testEntry.entryNum, bufferID);
                pthread_mutex_unlock(&(mutexes[bufferID]));
            }
            
        }
        
        entryNumber++;
        if (entryNumber > numEntries)
            entryNumber = 1;
    }
}

void *subscriber(void *args)
{
    int bufferID; // buffer id
    int threadID; // thread id
    // int threadState; // thread state

    threadID = ((struct subargs *) args)->id;
    // threadState = ((struct subargs *) args)->state;

    fprintf(stdout, "Subscriber(%d), %ld\n", threadID, pthread_self());
    while (1)
    {
        int lastEntry = buffers[bufferID].lastEntries[bufferID];
        for (bufferID = 0; bufferID < numBufs; bufferID++)
        {
            // Make subscriber wait till buffer is full
            if (!isFull(&buffers[bufferID]))
                sched_yield();

            pthread_mutex_lock(&(mutexes[bufferID]));

            if (GetEntry(&buffers[bufferID], lastEntry, &((struct subargs *) args)->tempEntry) == 1)
            {
                // ! printf("Last Entry: %d\n", lastEntries[bufferID]);
                fprintf(stdout, "Entry(%d) found in Buffer[%d].\n", ((struct subargs *) args)->tempEntry.entryNum, bufferID);
                pthread_mutex_unlock(&(mutexes[bufferID]));
                sleep(1);
            }
            else
            {
                fprintf(stdout, "Entry(%d) not found, waiting to try again.\n", lastEntry + 1);
                pthread_mutex_unlock(&(mutexes[bufferID]));
                sleep(1);
            }
            
        }

        buffers[bufferID].lastEntries[bufferID] = buffers[bufferID].lastEntries[bufferID] + 1;
        // ! printf("Last Entry: %d\n", buffers[bufferID].lastEntries[bufferID] + 1);
        if (buffers[bufferID].lastEntries[bufferID] > numEntries)
        {
            buffers[bufferID].lastEntries[bufferID] = 0;
        }
    }
}

void *cleanup_thread(void *args)
{
    int bufferID;
    int threadID;

    threadID = ((struct pubargs *) args)->id;
    fprintf(stdout, "Cleanup Thread(%d), %ld\n", threadID, pthread_self());

    struct timeval currTime;

    while (1)
    {
        for (bufferID = 0; bufferID < numBufs; bufferID++)
        {
            if (!isFull(&buffers[bufferID]))
                sched_yield();

            pthread_mutex_lock(&(mutexes[bufferID]));

            gettimeofday(&currTime, NULL);
            double timeDiff = difftime(currTime.tv_sec, buffers[bufferID].entries[buffers[bufferID].back].timeStamp.tv_sec);
            
            if (timeDiff >= deltaTime)
            {
                if (dequeue_routine(bufferID, &((struct subargs *) args)->tempEntry) == -1)
                {
                    fprintf(stdout, "Buffer[%d] is empty. Could not dequeue Entry\n", bufferID);
                    pthread_mutex_unlock(&(mutexes[bufferID]));
                    sleep(1);
                }
                else
                {
                    fprintf(stdout, "Cleanup_Thread: Entry(%d) popped off Buffer[%d].\n", ((struct subargs *) args)->tempEntry.entryNum, bufferID);
                    pthread_mutex_unlock(&(mutexes[bufferID]));
                }
            }
            else
            {
                pthread_mutex_unlock(&(mutexes[bufferID]));
            }
            
        }
    }
}
