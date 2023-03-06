# Starve-Free Reader-Writer Problem

The Reader-Writer Problem is a classic scenario in Computer Science where multiple processes concurrently access shared resources.The critical section can be accessed by only one writer or by multiple readers simultaneously at any point of time. Semaphores are used to prevent conflicts and ensure proper process synchronization. However, this approach may result in readers or writers being starved, depending on their priorities. In this context, I have proposed a solution to the Reader-Writer Problem that eliminates starvation.

## Data Structures, Functions and Semaphores Used :

To achieve process synchronization, semaphores are employed, which are associated with a critical section and have a queue (FIFO structure) that keeps track of blocked processes waiting to access the semaphore. When a process enters the `serviceQueue`, it becomes blocked, and once a process signals the semaphore, the process at the front of the `serviceQueue` is activated. The code below outlines the implementation of this approach.

### Process and FIFO structure
```cpp
// <SEMAPHORE CODE> //

//Process defined
struct process {
    process* next; //   pointing to the next process
    bool state = false; //   state represents (active if true) & (blocked/inactive if false)
    int ID = -1; //process ID
};

//Queue allowing FIFO semaphore
class serviceQueue {
    process *first, *last;
    int size = 0;
    int MAX = 50; // max number of waiting processes
    public:
        void push(int pid) { 
            if(size == MAX){
                return;
            }
            else{
                process* P;
                P->ID = pid;
                P->state = false; //  the process is blocked before pushing into the serviceQueue
                ++size;
                if(size == 0)
                    first = last = P;
                else
                    last = last->next = P;
            }
        }
        
        process* pop() {
            if(size == 0){
                return {nullptr,false,-1}; //   Returning Null process when empty
            }
            process* nextProcess = first;
            first = first->next;
            size--;
            return nextProcess;
        }
};
```
### Sempahores

Code for Semaphore
``` cpp
struct semaphore {
    int value = 1; //initial value of semaphore
    serviceQueue* service_queue = new serviceQueue();

    void signal(semaphore* s) {
        s->value++;
        if(s->value <= 0) {
            process* nextProcess = s->service_queue->pop();
            nextProcess->state = true; //  waking up the next process to be ready for entering critical section
        }
    }
    void wait(semaphore *s, int pid) {
        s->value--;
        if(s->value < 0)
            s->service_queue->push(pid); // blocking the process by pushing in service queue
    }
};
```
## Starvation??
Starvation is a problem when the low-priority process keeps waiting for a long duration of time because of high-priority requests being executed. A stream of high-priority requests stops the low-priority process from obtaining the processor or resources.

## Need for Starve-free solution
The Classical Solutions of Reader-Writer Problems assigns some priority to either the Readers or the Writer processes that leads to the starvation of the lower priority ones. This biased authority towards either the Writer or the Readers reduces the overall average response time and promotes inefficient utilization of resources. To solve this problem and distribute resources evenly accross all processes we need to make it Starve-free.

## Starve-Free Solution

The starve-free solution uses the `entryMutex` semaphore. The `entryMutex` must be acquired first by any reader or writer before accessing the `CSmutex` or entering the critical section directly. This solution solves the problem of starvation, as multiple readers arriving one after the other will no longer starve the writers. The `rmutex` here is used to give mutually exclusive access to the Rcount(Reader count) variable for the readers. In this approach, if a writer arrives in between two readers, and some readers are still in the critical section, the next reader cannot acquire the `entryMutex` as the writer has already acquired it. Once the existing readers leave the critical section, the writer who was waiting would be at the front of the serviceQueue for the `CSmutex` and would acquire it. This process would repeat, ensuring that both readers and writers have equal priority, and neither will starve. Moreover, this method preserves the advantage of readers not having to acquire the `CSmutex` every time another reader is present,resulting in a starvation-free solution to the Reader-Writer problem.

Here is the code implementation of the problem.

### Global Variables

Mainly 3 semaphores are employed here : CSmutex, rmutex and entryMutex

```cpp
// <INITIALIZATION> //
int Rcount = 0; //Reader count initially zero

semaphore* CSmutex = new semaphore(); // For access to critical section (either writer or readers)
semaphore* rmutex = new semaphore(); // Counting semaphore for readers
semaphore* entryMutex = new semaphore(); // The game-changing semaphore

//  At the start of both the reader and writer codes, the semaphore is employed and both READER/WRITER have equal priority to obtain it and enter the critical section
```
### Starve-Free Reader Implementation
```cpp
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
```

### Starve-Free Writer Implementation
The following is the implementation of the reader code using the starve-free approach.

```cpp
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
```
Thus the above problem solves the READER-WRITER problem with a starve-free heuristic.

### Details
Subhajit Biswas<br>
21114100<br>
B.Tech CSE 2Y<br>
IIT Roorkee
