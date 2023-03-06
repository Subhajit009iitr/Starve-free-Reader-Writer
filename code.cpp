// PSEUDOCODE //

// <INITIALIZATION> //
int Rcount = 0; //Reader count initially zero

semaphore* CSmutex = new semaphore(); // For access to critical section (either writer or readers)
semaphore* rmutex = new semaphore(); // Counting semaphore for readers
semaphore* entryMutex = new semaphore(); // The game-changing semaphore

//  At the start of both the reader and writer codes, the semaphore is employed and both READER/WRITER have equal priority to obtain it and enter the critical section

//  <STARVE-FREE READER CODE>
do{
void Reader(int processId) { //called when READER arrives
    
    wait(entryMutex, processId);
    //  the next section executes if the process with processId is not blocked by this semaphore
    wait(rmutex, processId);
    Rcount++;
    if(Rcount == 1) {   // accessed by the first reader
        wait(CSmutex, processId); //only writer will wait
    }
    signal(rmutex); // now readers that have acquired the entryMutex can access the Rcount
    signal(entryMutex); //  Once the reader is ready to enter the critical section, it frees the entryMutex semaphore which is acquired by a reader/writer whichever came first
        //  CRITICAL SECTION  //
        // The CS can be directly accessed by the upcoming readers if a reader is already inside it
    wait(rmutex, processId);
    Rcount--;
    signal(rmutex);
    if(Rcount == 0){
        signal(CSmutex); // the last reader frees the CSmutex and thus the critical section
    }
}
}while(true);

//  <STARVE-FREE WRITER CODE>
do{
void Writer(int processId) { // called when WRITER arrives
    wait(entryMutex, processId); //  the writer also waits for the entry mutex first which can be attained even when a reader is in CS
    wait(CSmutex, processId); //  the writer process would be blocked while waiting but once free it will acquire the CSmutex and enter the critical section
    signal(entryMutex); //  when the writer is ready to enter the critical section it frees the entryMutex which can be used by a reader/writer whichever came first
        //  CRITICAL SECTION  //
        //  the CS can be accessed by a writer only if they have attained the CSmutex
    signal(CSmutex); // writer frees the critical section 
    }
}while(true);
