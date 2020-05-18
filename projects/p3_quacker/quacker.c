/*
 * Seth Tal
 * 05/18/2020
 * Project 3 - Quacker...
*/


// Includes
#include "TopicEntry.c"


// Function declarations
void TestPart1();


// Main
int main()
{
    printf("\n");
    printf("Running program...\n\n");

    TestPart1();
    
    printf("\nDone.\n");
    return 1;
}

void TestPart1()
{
    EntryQueue *eQue = InitializeEntryQueue();

    for (int i = 0; i < eQue->capacity; i++)
    {
        TopicEntry currEntry;
        currEntry.entryNum = i + 1;
        EntryEnqueue(eQue, currEntry);
        fprintf(stdout, "Entry(%d) Enqueue'd.\n", currEntry.entryNum);
    }

    fprintf(stdout, "Test1: check array of entries for valid EntryEnqueue.\n\n");
    for (int i = 0; i < eQue->capacity; i++)
    {
        fprintf(stdout, "Entry(%d)\n", eQue->entries[i].entryNum);
    }

    fprintf(stdout, "\nTest2: test EntryDequeue() for proper dequeue-ing.\n\n");

    for (int i = 0; i < eQue->capacity; i++)
    {
        TopicEntry currEntry = EntryDequeue(eQue);
        fprintf(stdout, "Entry(%d) Dequeue'd.\n", currEntry.entryNum);
    }

    if (isEmpty(eQue))
        fprintf(stdout, "\nQue is now empty.\n\n");

    // fprintf(stdout, "Now testing entry values of Queue struct.\n\n");
    // for (int i = 0; i < eQue->capacity; i++)
    // {
    //     if (eQue->entries[i].entryNum != NULL)
    //         fprintf(stdout, "Entry(%d): %d\n", i+1, eQue->entries[i].entryNum);
    // }

    FreeEntryQueue(eQue);
}
