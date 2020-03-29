#include "PCB.h"

char getInput()
{
    char input = EOF; // User input for command
    printf("Enter a command: ");
    while ((input = getchar()) == '\n' || input == EOF) { }

    return toupper(input); // Always upper case
}

int main() {
    init();
    init_PCB();
    bool isRunning = true;
    int priority = 0;
    int pid = 0;
    char msg[40];
    char tempMsg;
    int semaphoreID = 0;
    int initialValue = 0;
    
    printf("Input \"B\" to break simulation.\n\n");
    while(isRunning) {
        switch (getInput()) {
            case 'C':
                // CREATE
                printf("Creating...\nEnter a priority (0 = low, 1 = Normal, 2 = High): ");
                scanf("%d", &priority);
                if(priority < 0 || priority > 2) {
                    printf("Invalid entry! Please enter a valid priority next time.\n\n");
                    break;
                }
                
                pid = create(priority);
                printf("New process with pid: %d created\n\n", pid);
                break;
                
            case 'F':
                // FORK
                
                // Exception: When no processes in allJobs
                if(ListCount(allJobs) == 0) {
                    printf("No jobs present to fork.\n\n");
                    break;
                }
                
                PCB_fork();
                break;
                
            case 'K':
                // Kill
                if(ListCount(allJobs) == 0) {
                    printf("No jobs present to kill.\n\n");
                    break;
                }
                printf("Enter pid to kill: ");
                scanf("%d", &pid);
                printf("\n\n");
                
                PCB_kill(pid);
                break;
                
            case 'E':
                // EXIT
                if (ListCount(readyJobs) == 0) {
                    printf("No running jobs to kill.\n\n");
                    break;
                }
                
                PCB_exit();
                break;
                
            case 'Q':
                // QUANTUM
                if (ListCount(allJobs) == 0) {
                    printf("No processes present for quantum to work.\n\n");
                    break;
                }
                
                PCB_quantum();
                break;
                
            case 'S':
                // SEND
                if (ListCount(allJobs) <= 1) {
                    printf("Not enough processes present to send to.\n\n");
                    break;
                }
                printf("Enter the process (pid) to send the message to: ");
                scanf("%d", &pid);
                printf("Enter a valid message (under 40 characters):\n");
                scanf("%c",&tempMsg); // temp statement to clear buffer
                scanf("%[^\n]",msg);
                printf("The msg is %s\n", msg);
//                while(fscanf(stdin, "%[^\n]%*c", msg) != EOF) { }
                
                PCBsend(pid, msg);
                printf("\n\n");
                break;
                
            case 'R':
                // RECEIVE
                if(ListCount(allJobs) <= 1) {
                    printf("Not enough processes present to receive a reply.\n\n");
                    break;
                }
                
                PCB_receive();
                break;
                
            case 'Y':
                // REPLY
                if (ListCount(allJobs) <= 0) {
                    printf("A reply cannot be made.\n\n");
                    break;
                }
                printf("Enter the process (pid) to send a reply to: ");
                scanf("%d", &pid);
                printf("Enter a valid message (under 40 characters): ");
                scanf("%c",&tempMsg); // temp statement to clear buffer
                scanf("%[^\n]",msg);
                
                PCB_reply(pid, msg);
                printf("\n\n");
                break;
                
            case 'N':
                // NEW SEMAPHORE
                if (ListCount(semaphores) > 5) {
                    printf("Failed to add another semaphore.\n\n");
                    break;
                }
                printf("Give an initial value for the semaphore: ");
                scanf("%d", &initialValue);
                semaphoreID = ListCount(semaphores);
                
                PCB_newSemaphore(semaphoreID, initialValue);
                printf("\n\n");
                break;
                
            case 'P':
                // SEMAPHORE P
                if(ListCount(semaphores) == 0) {
                    printf("No semaphores present.\n\n");
                    break;
                }
                printf("Enter the semaphore id for P operation: ");
                scanf("%d", &semaphoreID);
                
                PCB_semaphoreP(semaphoreID);
                printf("\n\n");
                break;
                
            case 'V':
                // SEMAPHORE V
                if(ListCount(semaphores) == 0) {
                    printf("No semaphores present.\n\n");
                    break;
                }
                printf("Enter the semaphore id for V operation: ");
                scanf("%d", &semaphoreID);
                
                PCB_semaphoreV(semaphoreID);
                printf("\n\n");
                break;
                
            case 'I':
                // PROCINFO
                if(ListCount(allJobs) == 0) {
                    printf("No processes to display.\n\n");
                    break;
                }
                printf("Enter the process (pid) to display on screen: ");
                scanf("%d", &pid);
                
                PCB_procInfo(pid);
                printf("\n\n");
                break;
                
            case 'T':
                // TOTALINFO
                if(ListCount(allJobs) == 0) {
                    printf("No processes to display.\n\n");
                    break;
                }
                
                PCB_totalInfo();
                break;
                
            case 'B':
                printf("Breaking simulation...\n");
                isRunning = false;
                break;
                
            default:
                printf("Invalid Command. Please try again\n");
                break;
        }
    }
    

    return 0;
}
