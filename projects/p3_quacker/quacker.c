/*
 * Seth Tal
 * 05/18/2020
 * Project 3 - Quacker server, a project in which we emulate a Publisher / Subscriber
 *             model for system communication. The server is a list of proxy threads,
 *             and functionality is emulated through file commands.
*/


// Includes --------------------------------------------------------------
#include "TopicEntry.c"


// Function declarations ------------------------------------------------
/* init(void)
 * This function initializes buffers, as well as any mutexes and pthread_attr.
*/
void init();

/* enqueue_routine(int, TopicEntry*)
 * Wrapper function for enqueuing entries into a Topic buffer.
*/
int enqueue_routine(int buffid, TopicEntry *entry);

/* dequeue_routine(int, TopicEntry*)
 * Wrapper function for dequeueing entries from a Topic buffer.
*/
int dequeue_routine(int buffid, TopicEntry *entry);

/* *publisher(void*)
 * The publisher thread handler
*/
void *publisher(void *args);

/* *subscriber(void*)
 * The subscriber thread handler
*/
void *subscriber(void *args);

/* *cleanup_thread(void*)
 * The cleanup thread handler
*/
void *cleanup_thread(void *args);

/* TokenizeLine(char*, char***, int)
 * A function I made for project 2, repurposed for this project.
 * Simply tokenizes a given string (tokens seperated by spaces).
*/
void TokenizeLine(char* toTokenize, char *** argArr, int arrLength);

/* ChangeThreadState(int, int, int)
 * A thread safe function to manage which threads are active.
*/
void ChangeThreadState(int val, int threadID, int threadType);
// -----------------------------------------------------------------------


// Global data -----------------------------------------------------------
EntryQueue buffers[MAXNUMBUFFERS];  // list of all Entry Queues
struct pubargs pubARGs[MAXPUBSUB];  // 
struct subargs subARGs[MAXPUBSUB];  // 
pthread_t pubs[NUMPROXIES];         // Thread ID for pubs
pthread_t subs[NUMPROXIES];         // Thread ID for subs
int pubStates[NUMPROXIES];          // states of each publisher thread
int subStates[NUMPROXIES];          // states of each subscriber thread

pthread_attr_t attr;                // Thread Attr

pthread_mutex_t mutexes[MAXNUMBUFFERS];
pthread_mutex_t threadStateMutex;

pthread_t cleanupID;
struct pubargs cleanupARGs;

double deltaTime = 1.0;
int numBufs = 0;

char subFileNames[NUMPROXIES][BUFSIZ];
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
/* Function remove_all_chars() taken from stack overflow user: dasblinkenlight
 * Link: https://stackoverflow.com/questions/9895216/how-to-remove-all-occurrences-of-a-given-character-from-string-in-c
*/
void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}
// -----------------------------------------------------------------------
/* usage(int)
 * This function checks for program usage errors.
*/
void usage(int argc)
{
    if (argc != 2)
    {
        fprintf(stderr, "USAGE: ./server <filename>\n");
        exit(EXIT_FAILURE);
    }
}

// Main ------------------------------------------------------------------
int main(int argc, char *argv[])
{
    usage(argc);

    printf("\n");
    printf("Running program...\n\n");

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        fprintf(stderr, "ERROR. an error occured opening file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char fileBuf[BUFSIZ];

    int bufferAmount = 0;
    int pubAmount = 0;
    int subAmount = 0;

    init();

    while (fgets(fileBuf, BUFSIZ, input))
    {
        if (strncmp(fileBuf, "create topic", 12) == 0)
        {
            // * Create a topic with id(int) and length.

            int numTokens = 1;
            for (unsigned long int j = 0; j < strlen(fileBuf); j++)
            {
                if (fileBuf[j] == ' ')
                    numTokens++;
            }

            char **tokenArray = NULL;
            TokenizeLine(fileBuf, &tokenArray, numTokens);

            buffers[bufferAmount].topicID = atoi(tokenArray[2]);
            // buffers[bufferAmount].topicName = tokenArray[3];
            remove_all_chars(tokenArray[3], '\"');
            strcpy(buffers[bufferAmount].topicName, tokenArray[3]);
            buffers[bufferAmount].capacity = atoi(tokenArray[4]);
            buffers[bufferAmount].back = buffers[bufferAmount].capacity - 1;

            free(tokenArray);
            bufferAmount++;
            numBufs++;
            if (bufferAmount >= MAXNUMBUFFERS)
            {
                bufferAmount = 0;
                numBufs--;
            }
        }

        else if (strncmp(fileBuf, "query topics", 12) == 0)
        {
            // * Print all topic ids and lengths

            for (int j = 0; j < bufferAmount; j++)
            {
                fprintf(stdout, "Topic(%d): len = %d, name = %s\n", buffers[j].topicID, buffers[j].capacity, buffers[j].topicName);
            }
        }

        else if (strncmp(fileBuf, "add publisher", 13) == 0)
        {
            // TODO - create publisher
            // * For now they should print:
            // *   "Proxy thread <thread id> - type: <Publisher/Subscriber>​"

            char **tokenArray = NULL;

            TokenizeLine(fileBuf, &tokenArray, 3);

            strncpy(pubARGs[pubAmount].filename, tokenArray[2], strlen(tokenArray[2]));
            
            remove_all_chars((char *)&pubARGs[pubAmount].filename, '\"');

            pubARGs[pubAmount].id = pubAmount;
            pubARGs[pubAmount].flag = 1;
            pthread_create(&pubs[pubAmount], &attr, publisher, (void *) &pubARGs[pubAmount]);

            free(tokenArray);
            pubAmount++;
        }

        else if (strncmp(fileBuf, "query publishers", 16) == 0)
        {
            // * Print out current publishers and their command filenames
            for (int i = 0; i < pubAmount; i++)
            {
                fprintf(stdout, "Publisher(%lu): %s\n", pubs[i], pubARGs[i].filename);
            }
        }

        else if (strncmp(fileBuf, "add subscriber", 14) == 0)
        {
            // TODO - create a subscriber
            // * For now they should print:
            // *   "Proxy thread <thread id> - type: <Publisher/Subscriber>​"

            char **tokenArray = NULL;

            TokenizeLine(fileBuf, &tokenArray, 3);

            strncpy(subARGs[subAmount].filename, tokenArray[2], strlen(tokenArray[2]));
            
            remove_all_chars((char *)&subARGs[subAmount].filename, '\"');

            char numBuf[2];
            sprintf(numBuf, "%d", subAmount);

            strcpy(subFileNames[subAmount], "subscriber-generated-");
            strcat(subFileNames[subAmount], (char *)numBuf);
            strcat(subFileNames[subAmount], ".html");

            // printf("%s\n", subFileNames[subAmount]);

            FILE  *subHTML = fopen(subFileNames[subAmount], "w");

            fprintf(subHTML, "<!DOCTYPE html>\n");
            fprintf(subHTML, "<html>\n");
            fprintf(subHTML, "<head>\n");
            fprintf(subHTML, "<title>Subscriber Generated Tables</title>\n");
            fprintf(subHTML, "<style>\n");

            fprintf(subHTML, "table, th, td { border: 1px solid black; border-collapse: collapse; }\n");
            fprintf(subHTML, "th, td { padding: 5px; }\n");
            fprintf(subHTML, "th { text-align: left; }\n");

            fprintf(subHTML, "</style>\n");
            fprintf(subHTML, "</head>\n");
            fprintf(subHTML, "<body>\n");
            fprintf(subHTML, "<h1>Subscriber: %s </h1>\n", subARGs[subAmount].filename);

            fclose(subHTML);

            subARGs[subAmount].id = subAmount;
            subARGs[subAmount].flag = 1;
            pthread_create(&subs[subAmount], &attr, subscriber, (void *) &subARGs[subAmount]);

            free(tokenArray);
            subAmount++;
        }

        else if (strncmp(fileBuf, "query subscribers", 17) == 0)
        {
            // * print out current subscribers and their command filenames

            for (int i = 0; i < subAmount; i++)
            {
                fprintf(stdout, "Subscriber(%lu): %s\n", subs[i], subARGs[i].filename);
            }
        }

        else if (strncmp(fileBuf, "delta", 5) == 0)
        {
            // * set DELTA to specified value
            
            char **tokenArray = NULL;
            TokenizeLine(fileBuf, &tokenArray, 2);

            deltaTime = (double) atoi(tokenArray[1]);

            free(tokenArray);
        }

        else if (strncmp(fileBuf, "start", 5) == 0)
        {
            printf("START\n");
            // TODO - start cleanup thread the all publisher and subscriber threads
            // Join main thread with pub + sub threads, start cleanup thread

            cleanupARGs.id = 0;
            pthread_create(&cleanupID, &attr, cleanup_thread, (void *) &cleanupARGs);

            for (int i = 0; i < pubAmount; i++)
            {
                ChangeThreadState(1, i, 1);
            }

            for (int i = 0; i < subAmount; i++)
            {
                ChangeThreadState(1, i, 0);
            }
        }

        else
        {
            fprintf(stderr, "ERROR. Unrecognized command inside input file.\n");
            exit(EXIT_FAILURE);
        }
        
    }

    fclose(input);

    pthread_join(cleanupID, NULL);

    for (int i = 0; i < pubAmount; i++)
    {
        pthread_join(pubs[i], NULL);
    }

    for (int i = 0; i < subAmount; i++)
    {
        pthread_join(subs[i], NULL);
    }

    // * finish off all of the subscriber html files
    for (int i = 0; i < subAmount; i++)
    {
        FILE *subHTML = fopen(subFileNames[i], "a");
        fprintf(subHTML, "</body>\n");
        fprintf(subHTML, "</html>\n");
        fclose(subHTML);
    }

    printf("\n\nDone.\n");
    return EXIT_SUCCESS;
} // main() --------------------------------------------------------------

void init()
{
    // Generate entry queues
    for (int i = 0; i < MAXNUMBUFFERS; i++)
    {
        buffers[i].size = 0;
        buffers[i].front = 0;

        for (int j = 0; j < numBufs; j++)
            buffers[i].lastEntries[j] = 0;
    }

    // generate buffer semaphores
    for (int i = 0; i < MAXNUMBUFFERS; i++)
    {
        pthread_mutex_init(&(mutexes[i]), NULL);
    }

    // init thread states
    for (int i = 0; i < NUMPROXIES; i++)
    {
        pubStates[i] = 0;
    }

    for (int i = 0; i < NUMPROXIES; i++)
    {
        subStates[i] = 0;
    }

    pthread_mutex_init(&threadStateMutex, NULL);

    pthread_attr_init(&attr);
}

void ChangeThreadState(int val, int threadID, int threadType)
{
    pthread_mutex_lock(&threadStateMutex);
    if (threadType == 1) // Publisher
    {
        pubStates[threadID] = val;
    }

    if (threadType == 0) // Subscriber
    {
        subStates[threadID] = val;
    }
    pthread_mutex_unlock(&threadStateMutex);
}

void TokenizeLine(char* toTokenize, char *** argArr, int arrLength)
{
    char** resultArgs;
    char* save = toTokenize;
    
    for (unsigned int i = 0; i < strlen(toTokenize); i++)
    {
        if (toTokenize[i] == '\n') {
            toTokenize[i] = '\0';
        }
    }
    
    resultArgs = (char**)malloc(sizeof(char*) * arrLength);

    char* currToken = strtok_r(save, " ", &save);

    int i = 0;
    while (currToken != NULL)
    {
        resultArgs[i] = currToken;
        i++;
        currToken = strtok_r(save, " ", &save);
    }

    *argArr = resultArgs;
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
    char file[BUFSIZ];
    strcpy(file, ((struct pubargs *) args)->filename);
    // int threadState; // thread state

    threadID = ((struct pubargs *) args)->id;
    // threadState = ((struct pubargs *) args)->state;

    FILE *commands = fopen(file, "r");
    if (commands == NULL)
    {
        fprintf(stderr, "ERROR. an error occured when opening file: %s\n", file);
        exit(EXIT_FAILURE);
    }

    char line[BUFSIZ];
    int topicEntryNum = 1;

    while (1)
    {
        if (pubStates[threadID] == 1)
        {
            fprintf(stdout, "Proxy thread %d - type: Publisher(%lu) ~ started.\n", threadID, pthread_self());

            while (fgets(line, BUFSIZ, commands))
            {
                if (strncmp(line, "put", 3) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Publisher - Executed Command: PUT\n", threadID);


                    char **tokenArray = NULL;
                    TokenizeLine(line, &tokenArray, 4);

                    bufferID = atoi(tokenArray[1]);

                    TopicEntry newEntry;
                    newEntry.entryNum = topicEntryNum;
                    remove_all_chars(tokenArray[2], '\"');
                    remove_all_chars(tokenArray[3], '\"');
                    strcpy(newEntry.photoURL, tokenArray[2]);
                    strcpy(newEntry.photoCaption, tokenArray[3]);
                    newEntry.pubID = threadID;

                    pthread_mutex_lock(&mutexes[bufferID - 1]);

                    if (enqueue_routine(bufferID - 1, &newEntry) == -1)
                    {
                        // fprintf(stdout, "Publisher(%d), Buffer[%d] is full.\n", threadID, bufferID);
                        pthread_mutex_unlock(&(mutexes[bufferID - 1]));

                    }
                    else
                    {
                        // fprintf(stdout, "Publisher(%d), Entry(%d) pushed onto Buffer[%d]\n", threadID, newEntry.entryNum, bufferID);
                        pthread_mutex_unlock(&(mutexes[bufferID - 1]));
                    }
                    
                    topicEntryNum++;
                    if (topicEntryNum > buffers[bufferID - 1].capacity)
                        topicEntryNum = 1;

                    free(tokenArray);
                }

                else if (strncmp(line, "sleep", 5) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Publisher - Executed Command: SLEEP\n", threadID);


                    char **tokenArray = NULL;
                    TokenizeLine(line, &tokenArray, 2);

                    usleep(atoi(tokenArray[1]) * 1000);

                    free(tokenArray);
                }

                else if (strncmp(line, "stop", 4) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Publisher - Executed Command: STOP\n", threadID);

                    ChangeThreadState(0, threadID, 1);
                    fclose(commands);
                    pthread_exit(NULL);
                }

                else
                {
                    fprintf(stderr, "ERROR. unrecognized command in file: %s\n.", file);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    fclose(commands);
}

void AddTopicNameToHTML(char filename[], char topicName[])
{
    FILE *subHTML = fopen(filename, "a");
    fprintf(subHTML, "<h2>Topic Name: %s</h2>\n", topicName);
    fprintf(subHTML, "<table style=\"width:100%\" align=\"middle\">\n");
    fprintf(subHTML, "<tr><th>CAPTION</th><th>PHOTO-URL</th></tr>\n");
    fclose(subHTML);
}

void AddTableToHTML(char filename[], TopicEntry entry)
{
    FILE *subHTML = fopen(filename, "a");
    fprintf(subHTML, "<tr><th>%s</th><th>%s</th></tr>", entry.photoCaption, entry.photoURL);
    fclose(subHTML);
}

void CloseTableHTML(char filename[])
{
    FILE *subHTML = fopen(filename, "a");
    fprintf(subHTML, "</table>\n");
    fclose(subHTML);
}

void *subscriber(void *args)
{
    int bufferID; // buffer id
    char file[BUFSIZ];
    int threadID = ((struct subargs *) args)->id;
    strcpy(file, ((struct subargs *) args)->filename);

    int seenTopics[numBufs];
    for (int i = 0; i < numBufs; i++)
    {
        seenTopics[i] = 0;
    }

    FILE *commands = fopen(file, "r");
    if (commands == NULL)
    {
        fprintf(stderr, "ERROR. an error occured when opening file: %s\n", file);
        exit(EXIT_FAILURE);
    }

    char line[BUFSIZ];

    while (1)
    {
        if (subStates[threadID] == 1)
        {
            fprintf(stdout, "Proxy thread %d - type: Subscriber(%lu) ~ started.\n", threadID, pthread_self());
#if 1
            while (fgets(line, BUFSIZ, commands))
            {
                if (strncmp(line, "get", 3) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Subscriber - Executed Command: GET\n", threadID);

                    char **tokenArray = NULL;
                    TokenizeLine(line, &tokenArray, 2);

                    bufferID = atoi(tokenArray[1]);
                    
                    if (seenTopics[bufferID - 1] == 0 && !isEmpty(&buffers[bufferID - 1]))
                    {
                        AddTopicNameToHTML(subFileNames[threadID], buffers[bufferID - 1].topicName);
                        seenTopics[bufferID - 1] = 1;
                    }
                    // printf("seen topic(%d) - %d\n", bufferID, seenTopics[bufferID - 1]);
                    int tryCount = 0;
                    while (buffers[bufferID - 1].lastEntries[bufferID - 1] < buffers[bufferID - 1].capacity)
                    {
                        if (tryCount > 2)
                        {
                            fprintf(stdout, "\tBuffer[%d] exceeded max attempts to grab entry, Moving on...\n", bufferID);
                            break;
                        }
                        pthread_mutex_lock(&mutexes[bufferID - 1]);

                        int ge_flag = GetEntry(&buffers[bufferID - 1], buffers[bufferID - 1].lastEntries[bufferID - 1], &((struct subargs *) args)->tempEntry);

                        if (ge_flag == 1)
                        {
                            ((struct subargs *) args)->tempEntry.pubID = threadID;
                            fprintf(stdout, "\n\tEntry(%d): \n\tlocation - Buffer[%d]\n\tpubID - %d\n\tURL - %s\n\tCaption - %s\n\n", ((struct subargs *) args)->tempEntry.entryNum, bufferID, ((struct subargs *) args)->tempEntry.pubID, ((struct subargs *) args)->tempEntry.photoURL, ((struct subargs *) args)->tempEntry.photoCaption);
                            AddTableToHTML(subFileNames[threadID], ((struct subargs *) args)->tempEntry);
                            pthread_mutex_unlock(&mutexes[bufferID - 1]);
                        }

                        else if (ge_flag == 0)
                        {
                            fprintf(stdout, "\tBuffer[%d] next entry not found. Trying again...\n", bufferID);
                            pthread_mutex_unlock(&(mutexes[bufferID - 1]));
                            usleep(500 * 1000);  // wait another half a second before checking again
                            buffers[bufferID - 1].lastEntries[bufferID - 1] = buffers[bufferID - 1].lastEntries[bufferID - 1] + 1;
                            tryCount++;
                            continue;
                        }

                        else
                        {
                            fprintf(stdout, "\n\tEntry(%d) not found. Entry(%d) found instead...\n", buffers[bufferID - 1].lastEntries[bufferID - 1] + 1, ((struct subargs *) args)->tempEntry.entryNum);
                            ((struct subargs *) args)->tempEntry.pubID = threadID;
                            fprintf(stdout, "\n\t\tEntry(%d): \n\t\tlocation - Buffer[%d]\n\t\tpubID - %d\n\t\tURL - %s\n\t\tCaption - %s\n\n", ((struct subargs *) args)->tempEntry.entryNum, bufferID, ((struct subargs *) args)->tempEntry.pubID, ((struct subargs *) args)->tempEntry.photoURL, ((struct subargs *) args)->tempEntry.photoCaption);
                            AddTableToHTML(subFileNames[threadID], ((struct subargs *) args)->tempEntry);
                            buffers[bufferID - 1].lastEntries[bufferID - 1] = ((struct subargs *) args)->tempEntry.entryNum - 1;
                            pthread_mutex_unlock(&(mutexes[bufferID - 1]));
                        }

                        buffers[bufferID - 1].lastEntries[bufferID - 1] = buffers[bufferID - 1].lastEntries[bufferID - 1] + 1;
                    }

                    if (buffers[bufferID - 1].lastEntries[bufferID - 1] > buffers[bufferID - 1].capacity)
                    {
                        buffers[bufferID - 1].lastEntries[bufferID - 1] = 0;
                    }

                    CloseTableHTML(subFileNames[threadID]);
                    free(tokenArray);
                }

                else if (strncmp(line, "sleep", 5) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Subscriber - Executed Command: SLEEP\n", threadID);
                    // fprintf(stdout, "cmd: SLEEP\n");

                    char **tokenArray = NULL;
                    TokenizeLine(line, &tokenArray, 2);

                    usleep(atoi(tokenArray[1]) * 1000);

                    free(tokenArray);
                }

                else if (strncmp(line, "stop", 4) == 0)
                {
                    fprintf(stdout, "Proxy thread %d - type: Subscriber - Executed Command: STOP\n", threadID);
                    // fprintf(stdout, "cmd: STOP\n");
                    ChangeThreadState(0, threadID, 0);
                    fclose(commands);
                    pthread_exit(NULL);
                }

                else
                {
                    fprintf(stderr, "ERROR. unrecognized command in file: %s\n.", file);
                    exit(EXIT_FAILURE);
                }
            }
#endif
        }
    }
    fclose(commands);

    for (int i = 0; i < numBufs; i++)
    {
        seenTopics[i] = 0;
    }
}

void *cleanup_thread(void *args)
{
    fprintf(stdout, "Cleanup Thread(%lu) ~ started.\n", pthread_self());

    int bufferID;
    while (1)
    {
        for (bufferID = 0; bufferID < numBufs; bufferID++)
        {
            if (isEmpty(&buffers[bufferID]))
                continue;

            pthread_mutex_lock(&(mutexes[bufferID]));

            struct timeval currTime;
            gettimeofday(&currTime, NULL);
            // double timeDiff = difftime(currTime.tv_sec, buffers[bufferID].entries[buffers[bufferID].front].timeStamp.tv_sec);
            double timeDiff = currTime.tv_sec - buffers[bufferID].entries[buffers[bufferID].front].timeStamp.tv_sec;
            // printf("buffer[%d] - current: %ld, timestamp: %ld\n", bufferID, currTime.tv_sec, buffers[bufferID].entries[buffers[bufferID].front].timeStamp.tv_sec);
            // fprintf(stdout, "%lf vs. %lf, entry: %d\n", deltaTime, timeDiff, buffers[bufferID].entries[buffers[bufferID].back].entryNum);
            if (timeDiff >= deltaTime)
            {
                // fprintf(stdout, "%lf vs. %lf, entry: %d\n", deltaTime, timeDiff, buffers[bufferID].entries[buffers[bufferID].back].entryNum);
                if (dequeue_routine(bufferID, &((struct subargs *) args)->tempEntry) == -1)
                {
                    fprintf(stdout, "Cleanup Thread ~ Buffer[%d] is empty\n", bufferID);
                    pthread_mutex_unlock(&(mutexes[bufferID]));
                }
                else
                {
                    // printf("current: %ld, timestamp: %ld\n", currTime.tv_sec, buffers[bufferID].entries[buffers[bufferID].back].timeStamp.tv_sec);
                    fprintf(stdout, "Cleanup_Thread: Entry(%d) popped off Buffer[%d].\n", ((struct subargs *) args)->tempEntry.entryNum, bufferID);
                    pthread_mutex_unlock(&(mutexes[bufferID]));
                }
            }
            else
            {
                pthread_mutex_unlock(&(mutexes[bufferID]));
            }
            
        }

        int inactivePubs = 0;
        int inactiveSubs = 0;
        for (int i = 0; i < NUMPROXIES; i++)
        {
            if (pubStates[i] == 0)
                inactivePubs++;
            if (subStates[i] == 0)
                inactiveSubs++;
        }

        if (inactivePubs >= NUMPROXIES && inactiveSubs >= NUMPROXIES)
        {
            fprintf(stdout, "Cleanup Thread terminating.\n");
            pthread_exit(NULL);
        }
    }
}
