#if 0
void TestPart1()
{
    fprintf(stdout, "\nTesting Components of Part 1...\n\n");
    EntryQueue *eQue = InitializeEntryQueue();

    for (int i = 0; i < eQue->capacity; i++)
    {
        TopicEntry currEntry;
        currEntry.entryNum = i + 1;
        EntryEnqueue(eQue, &currEntry);
        fprintf(stdout, "Entry(%d) Enqueue'd.\n", currEntry.entryNum);
        // fprintf(stdout, "TimeStamp: %ld\n", currEntry.timeStamp.tv_sec);
    }

    fprintf(stdout, "\nTest1: check array of entries for valid EntryEnqueue.\n\n");
    for (int i = 0; i < eQue->capacity; i++)
    {
        fprintf(stdout, "Entry(%d)\n", eQue->entries[i].entryNum);
        fprintf(stdout, "TimeStamp: %ld\n", eQue->entries[i].timeStamp.tv_sec);
    }

    fprintf(stdout, "\nTest2: check head and tail pointers of queue struct.\n\n");
    fprintf(stdout, "Head -> Entry(%d)\n", eQue->head->entryNum);
    fprintf(stdout, "Tail -> Entry(%d)\n", eQue->tail->entryNum);

    fprintf(stdout, "\nTest3: test EntryDequeue() for proper dequeue-ing.\n\n");

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
    fprintf(stdout, "...Testing for Part 1, Done.\n\n");
}

void OtherTests()
{
    EntryQueue *eQue = InitializeEntryQueue();

    for (int i = 0; i < 5; i++)
    {
        TopicEntry currEntry;
        currEntry.entryNum = i+1;
        EntryEnqueue(eQue, &currEntry);
        fprintf(stdout, "Entry(%d), Enqueue'd @ time: %ld\n\n", eQue->entries[eQue->front+i].entryNum, eQue->entries[eQue->front+i].timeStamp.tv_sec);
    }
    printf("Size of Queue: %d\n\n", eQue->size);

    TopicEntry toDequeue = EntryDequeue(eQue);
    fprintf(stdout, "Entry(%d), Dequeue'd.\n\n", toDequeue.entryNum);

    printf("Size of Queue: %d\n\n", eQue->size);

    printf("Que:\n");
    for (int i = 0; i < eQue->size; i++)
    {
        fprintf(stdout, "Entry(%d)\n", eQue->entries[eQue->front+i].entryNum);
    }

    TopicEntry testGetEntry;

    if (GetEntry(eQue, 3, &testGetEntry) == 1)
    {
        fprintf(stdout, "\nEntry(%d) found in queue.\n\n", testGetEntry.entryNum);
    }

    FreeEntryQueue(eQue);
}
#endif
