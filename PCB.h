#include<stdbool.h>
#include "List.h"

#define RUNNING 2
#define READY 1
#define BLOCKED 0
#define DEADLOCKED -1

//PCB structure definition
typedef struct {
    int pid;        // Process ID
    int priority;   // 0 = low, 1 = Normal, 2 = High
    int state;      // -1 = deadlocked, 0 = Blocked, 1 = Ready, 2 = Running
    char *proc_message;   // Allow a message to be sent or received
} PCB;

// Semaphores Data structure
typedef struct {
    int value;
    LIST* waiting;
} SEMAPHORE;

//Lists
LIST* lowP;
LIST* normalP;
LIST* highP;
LIST* sending; // contains items that are blocked because of sending and waiting to receive
LIST* receiving;
LIST* allJobs;
LIST* readyJobs;
LIST semaphores[5]; // MAX 5

// Required functions
int create(int priority);
int PCB_fork(void);
int PCB_kill(int pid);
void PCB_exit(void);
void PCB_quantum(void);
int PCBsend(int pid, char* msg);
void PCB_receive(void);
int PCB_reply(int pid, char* msg);

int PCB_newSemaphore(int semaphoreID, int initialValue);
int PCB_semaphoreP(int semaphoreID);
int PCB_semaphoreV(int semaphoreID);

void PCB_procInfo(int pid);
void PCB_totalInfo(void);

// Function for initialization of all the LISTS
void init_PCB(void);


//------------------------------------------------------------------------

void init_PCB(void) {
    lowP = ListCreate();
    normalP = ListCreate();
    highP = ListCreate();
    sending = ListCreate();
    receiving = ListCreate();
    allJobs = ListCreate();
    readyJobs = ListCreate();
    for(int i=0; i<5; i++) {
        semaphores[i].count=0;
        semaphores[i].curr = NULL;
        semaphores[i].first = NULL;
        semaphores[i].last = NULL;
        semaphores[i].isFree = 1;
    }
}

int create(int priority) {
    PCB* block = (PCB*)malloc(sizeof(PCB)); //Use malloc since size of processes is uncertain
    
    // Assign pid
    block->pid = ListCount(allJobs) + 1;
    block->priority = priority;
    
    ListAppend(allJobs, block);
    
    // Decide state (Ready, Running, Deadlocked or Blocked)
    if(ListCount(allJobs) == 1) {
        block->state = RUNNING;
    }
    else {
        block->state = READY;
        ListPrepend(readyJobs, block);
    }
    
    // Place in priority queue
    if(priority == 0) {
        ListAppend(lowP, block);
    }
    else if(priority == 1) {
        ListAppend(normalP, block);
    }
    else {
        ListAppend(highP, block);
    }
    
    int count = ListCount(allJobs);
    printf("Number of jobs in Queue: %d\n", count);
    
    return block->pid;
}

int PCB_fork(void) {
    Node* currentProcess = ListFirst(allJobs);  // Start with the first process on the List of all Jobs
    PCB* newBlock = (PCB*)currentProcess->data; // New process (pid to be returned)
    
    // Get the process which is running
    while(newBlock->state != RUNNING) {
        currentProcess = currentProcess->next;
        newBlock = (PCB*) currentProcess->data;
    }
    
    // create() returns the pid of the new process
    int newPid = create(newBlock->priority);
    
    printf("Fork Created with id = %d.\n", newPid);
    return newPid;
}

PCB* getNextReady(void) {
    PCB* retBlock = (PCB*)malloc(sizeof(PCB));
    Node* head = (Node*)malloc(sizeof(Node));
    
    if(highP!=NULL) {
        head = readyJobs->first;
        retBlock = (PCB*) head->data;
        while (head->next!=NULL) {
            if(retBlock->state == READY && retBlock->priority == 2) {
                readyJobs->curr = head;
                ListRemove(readyJobs);
                return retBlock;
            }
            head = head->next;
            retBlock = (PCB*) head->data;
        }
    } else if (normalP != NULL) {
        head = readyJobs->first;
        retBlock = (PCB*) head->data;
        while (head->next!=NULL) {
            if(retBlock->state == READY && retBlock->priority == 1) {
                readyJobs->curr = head;
                ListRemove(readyJobs);
                return retBlock;
            }
            head = head->next;
            retBlock = (PCB*) head->data;
        }
    } else if(lowP != NULL) {
        head = readyJobs->first;
        retBlock = (PCB*) head->data;
        while (head->next!=NULL) {
            if(retBlock->state == READY && retBlock->priority == 0) {
                readyJobs->curr = head;
                ListRemove(readyJobs);
                return retBlock;
            }
            head = head->next;
            retBlock = (PCB*) head->data;
        }
    } else {
        // No process available
        return NULL;
    }
    
    return NULL;
}

int PCB_kill(int pid) {
    Node* process = ListFirst(allJobs);
    PCB* killBlock = (PCB*) process->data;
    int killID = killBlock->pid;
    
    // Check if the named process exists
    while(killID != pid && process->next != NULL) {
        if(killID != pid && process == allJobs->last) {
            printf("The entered process ID does not exist.\n");
            return 0; // FAIL
        }
        process = process->next;
        killBlock = (PCB*) process->data;
        killID = killBlock->pid;
    }
    
    if(killID != pid) {
        printf("The entered process ID does not exist.\n");
        return 0; // FAIL
    } else {
        allJobs->curr = process;
        
        // Make the next ready process run if the one to be killed is RUNNING
        if(readyJobs->count > 0) {
            PCB* tempBlock = getNextReady();
        
            if(tempBlock != NULL) {
                if(killBlock->state == RUNNING) {
                    tempBlock->state = RUNNING;
                }
            }
        }
        ListRemove(allJobs); // Remove current
    }
    
    return 1;
}

void PCB_exit(void)
{
    // find the currently running process and kill it.
    Node* process = ListFirst(allJobs);
    PCB* exitBlock = (PCB*) process->data;
    int state = exitBlock->state;
    
    while(state != RUNNING) {
        if(exitBlock->pid == ListCount(allJobs)) {
            printf("No processes running currently.");
            return;
        }
        
        process = process->next;
        exitBlock = (PCB*) process->data;
        state = exitBlock->state;
    }
    
    PCB_kill(exitBlock->pid);
    
    return;
}

void PCB_quantum(void) {
    // Currently running process runs out of time
    // change the state to "READY"
    
    Node* currentRunning = ListFirst(allJobs);
    PCB* readyBlock = (PCB*) currentRunning->data;
    int state = readyBlock->state;
    
    while(state != RUNNING && currentRunning->next != NULL) {
        currentRunning = currentRunning->next;
        readyBlock = (PCB*) currentRunning->data;
        state = readyBlock->state;
    }
    printf("Process currently running:\n");
    PCB_procInfo(readyBlock->pid);
    
    readyBlock->state = READY;
    ListPrepend(readyJobs, readyBlock);
    
    printf("This process has been removed from CPU. \nThe next process now running:\n");
    
    // There will always be atleast one ready job (most recent one)
    PCB* tmp = ListTrim(readyJobs);
    tmp->state = RUNNING;
    PCB_procInfo(tmp->pid);
    
    return;
}

int PCBsend(int pid, char* msg) {
    // pid is the id of receiving process
    
    Node* receivingProc = ListFirst(allJobs);
    PCB* rBlock = (PCB*) receivingProc->data;
    int rid = rBlock->pid;
    
    while(rid != pid) {
        if(ListLast(allJobs) == receivingProc && rid != pid) {
            printf("Cannot find receiving item.\n");
            return 0;
        }
        receivingProc = receivingProc->next;
        rBlock = (PCB*) receivingProc->data;
        rid = rBlock->pid;
    }
    
    // find the sending process;
    // sending process should be the one "RUNNING"
    Node* sendingProc = ListFirst(allJobs);
    PCB* sBlock = (PCB*) sendingProc->data;
    int state = sBlock->state;
    
    while(state != RUNNING) {
        if(sBlock->pid == ListCount(allJobs) && state != RUNNING) {
            printf("Cannot find sending item.\n");
            return 0;
        }
        
        sendingProc = sendingProc->next;
        sBlock = (PCB*) sendingProc->data;
        state = sBlock->state;
    }
    
    ListPrepend(receiving, sBlock);
    if(rBlock->state != BLOCKED) {
        rBlock->proc_message = msg;
        printf("Message received: %s", msg);
    }
    else {
        if(rBlock->proc_message == NULL) {
            rBlock->state = READY;
            ListPrepend(readyJobs, rBlock);
            printf("Receiuving job was blocked without a message.\nIt is now on ready queue.\n");
            return 1;
        }
    }
    
    // Sender process is BLOCKED
    sBlock->state = BLOCKED;
    printf("Sending process is now blocked until it gets a reply.\n");
    PCB_procInfo(sBlock->pid);
    ListAppend(sending, sBlock);
    
    if(ListCount(readyJobs) > 0) {
        PCB* nextJob = getNextReady();
        nextJob->state = RUNNING;
        printf("Next ready job is running.\n");
    } else {
        printf("No more ready jobs available.\n");
    }
    
    return 1;
}

void PCB_receive(void) {
    // find the currently executing process
    Node* process = ListFirst(allJobs);
    PCB* rBlock = (PCB*) process->data;
    int state = rBlock->state;
    
    while(state != RUNNING) {
        if(rBlock->pid == ListCount(allJobs)) {
            printf("No process running. No receive possible.\n");
            return;
        }
        
        process = process->next;
        rBlock = (PCB*) process->data;
        state = rBlock->state;
    }
    
    if(rBlock->proc_message == NULL) {
        rBlock->state = BLOCKED;
        if(process->next != NULL) {
            rBlock = (PCB*) process->next->data;
            rBlock->state = RUNNING;
            return;
        }
    }
    else {
        printf("Message received\n");
    }
    
    return;
}

int PCB_reply(int pid, char* msg) {
    // find process with pid (sender)
    Node* sendingProc = ListFirst(allJobs);
    PCB* sBlock = (PCB*) sendingProc->data;
    int sid = sBlock->pid;
    
    while(sid != pid) {
        if(ListLast(allJobs) == sendingProc && sid != pid) {
            return 0;
        }
        sendingProc = sendingProc->next;
        sBlock = (PCB*) sendingProc->data;
        sid = sBlock->pid;
    }
    
    sBlock->state = READY;
    sBlock->proc_message = msg;
    
    return 1;
}

int PCB_newSemaphore(int semaphoreID, int initialValue) {
    SEMAPHORE* sem = (SEMAPHORE*)malloc(sizeof(SEMAPHORE));
    // No processes waiting initially
    sem->waiting = NULL;
    sem->value = initialValue;
    LIST* newSemaphore = NULL;
    
    if(!semaphores[semaphoreID].isFree) {
        // Fail case
        printf("Failed to create a new semaphore. One already exists at position: %d\n", semaphoreID);
        return 0;
    } else {
        newSemaphore = &semaphores[semaphoreID];
        semaphores[semaphoreID].isFree = 0;
        ListAppend(newSemaphore, sem);
        printf("Semaphore ID: %d\n", semaphoreID);
        printf("Semaphore initial value: %d\n", initialValue);
    }
    
    return 1;
}

int PCB_semaphoreP(int semaphoreID) {
    if(semaphoreID>4) {
        printf("Semaphore id out of bounds\n");
        // Fail
        return 0;
    } else if(semaphores[semaphoreID].isFree) {
        // Fail
        return 0;
    }
    
    Node* head = semaphores[semaphoreID].first;
    SEMAPHORE* sem = (SEMAPHORE*) head->data;
    
    // Find the currently RUNNING process
    Node* currentRunning = ListFirst(allJobs);
    PCB* readyBlock = (PCB*) currentRunning->data;
    int state = readyBlock->state;
    
    while(state != RUNNING) {
        currentRunning = currentRunning->next;
        readyBlock = (PCB*) currentRunning->data;
        state = readyBlock->state;
    }
    
    if(sem->value > 0) {
        sem->value -= 1;
        return 1;
    } else {
        // Add process to waiting queue
        ListPrepend(sem->waiting, readyBlock);
        readyBlock->state = BLOCKED;
    }
    
    return 2;
}

int PCB_semaphoreV(int semaphoreID) {
    if(semaphoreID>4) {
        printf("Semaphore id out of bounds\n");
        // Fail
        return 0;
    } else if(semaphores[semaphoreID].isFree) {
        // Fail
        return 0;
    }
    
    Node* head = semaphores[semaphoreID].first;
    SEMAPHORE* sem = (SEMAPHORE*) head->data;
    
    sem->value += 1;
    if(sem->value <= 0) {
        if(sem->waiting != NULL) {
            PCB* receiveBlock = ListTrim(sem->waiting);
            receiveBlock->state = READY;
            ListPrepend(readyJobs, receiveBlock);
        }
    } else {
        return 1;
    }
    
    return 2;
}

void PCB_procInfo(int pid) {
    Node* process = ListFirst(allJobs);
    PCB* infoBlock = (PCB*) process->data;
    int infoID = infoBlock->pid;
    
    // Check if pid is valid
    while(infoID != pid && process->next != NULL) {
        infoBlock = (PCB*) process->data;
        infoID = infoBlock->pid;
        
        if(infoID != pid && process == allJobs->last) {
            printf("Invalid pid entered!\n");
            return; // FAIL
        }
        
        process = process->next;
    }
    
    printf("Selected process:\n");
    printf("PID: %d, Priority: %d, State: %d \n", infoBlock->pid, infoBlock->priority, infoBlock->state);
    
    return;
}

void PCB_totalInfo(void) {
    Node* process = ListFirst(allJobs);
    PCB* block;
    
    printf("Displaying all Jobs:\n");
    
    while(process != NULL) {
        
        block = (PCB*) process->data;
        printf("PID: %d, Priority: %d, State: %d \n", block->pid, block->priority, block->state);
        process = process->next;

    }
    
    return;
}
