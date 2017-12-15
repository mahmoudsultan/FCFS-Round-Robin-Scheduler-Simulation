/**
 * FCFS Scheduler Algorithm Simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include "fcfs.h"

#define DEBUG_MODE 1

void enque(ProcessQueue *q, Process *process) {
    if (q == NULL) {
        fprintf(stderr, "Queue is Null");
        return;
    }
    
    ProcessQueueNode *processQueueNode = malloc(sizeof(ProcessQueueNode));
    processQueueNode->process = process;
    
    processQueueNode->next = NULL;
    // if queue is empty
    if (q->head == NULL) {
        q->head = processQueueNode;
        q->tail = processQueueNode;
    } else {
        q->tail->next = processQueueNode;
        q->tail = processQueueNode;
    }
}

Process* deque(ProcessQueue *q) {
    if (q->head == NULL) {
        return NULL;
    }
    
    Process* temp = q->head->process;
    ProcessQueueNode* temp_node = q->head;
    
    if (q->head == q->tail) {
        q->head = q->tail = NULL;
    } else {
        q->head = q->head->next;
    }
    
    free(temp_node);
    
    if (DEBUG_MODE) {
        ProcessQueueNode *ptr = q->head;
        while(ptr != NULL) {
//            printf("DEBUG: FCFS: QUEUE AFTER DEQUE: node id %d \n", ptr->process->process_id);
            ptr = ptr->next;
        }
    }
    
    return temp;
}

Process* peak(ProcessQueue *q) {
    return q->head->process;
}

void build_process_queue(ProcessQueue *q, Process **process_list) {
    Process *ptr = *process_list;
    int count = 0;
    while (ptr != NULL) {
        enque(q, ptr);
        ptr = process_list[++count];
    }
    
    if (DEBUG_MODE) {
        ProcessQueueNode *ptr = q->head;
        while(ptr != NULL) {
//            printf("DEBUG: FCFS: ENQUEUE: node id %d \n", ptr->process->process_id);
            ptr = ptr->next;
        }
    }
}

/**
 * Iterates over the process list and build a list of processes that
 * arrived to the system at this tick
 * returns that count of those processes
 */
int get_arrived_processes(Process** arrived, Process** process_list, int tick) {
    Process *ptr = *process_list;
    int count = 0, arrived_count = 0;
    while (ptr != NULL) {
        if (ptr->arrival_time == tick) {
            ptr->status = READY;
            ptr->turnaround = tick;
            arrived[arrived_count] = ptr;
            arrived_count++;
        }
        ptr = process_list[++count];
    }
    arrived[arrived_count] = NULL;
    return arrived_count;
}

Process* choose_running_process(ProcessQueue* q, int tick) {
    if (q->head == NULL) {
        return NULL;
    }
    
    Process* top_process = peak(q);
    if (top_process->arrival_time > tick) {
        // arival time not yet reached
        return NULL;
    } else {
        deque(q);
        top_process->status = RUNNING;
    }
    return NULL;
}

int run_fcfs(Process **process_list, int no_of_processes) {
    // open file to keep output
    FILE* f = fopen("FCFS.out", "w");
    
    // keep a queue of ready processes
    ProcessQueue q;
    q.head = q.tail = NULL;
    
    // keep a list of IO Blocked processes
    Process** blocked_processes = malloc(sizeof(Process) * (no_of_processes - 1));
    int blocked_count = 0;
    
    // keep track of running process
    Process* running = NULL;
    
    // keeps track of terminated no of processes
    int terminated_count = 0;
    
    int i;
    Process* temp;
    
    // start cpu ticking
    int tick = 0, not_utilized_count = 0;
    
    Process** to_be_enqued = malloc(sizeof(Process) * (no_of_processes - 1));
    int to_be_enqued_count = 0;
    
    // while there's a process either in ready queue or blocked queue
    while(terminated_count != no_of_processes) {
        for (i = 0; i < to_be_enqued_count; i++)
            to_be_enqued[i] = NULL;
        to_be_enqued_count = 0;
        
        // get processes that arrived to the system
        to_be_enqued_count += get_arrived_processes(to_be_enqued, process_list, tick);
        if (DEBUG_MODE) {
            printf("DEBUG: tick %d - %d arrived \n", tick, to_be_enqued_count);
        }
        
        /* Iterate over blocked processes and increase spent io time
         * if the process finished its io time add it to the to be enqued
         * list, and change its status to READY
         */
        i = 0;
        while(i < blocked_count) {
            temp = blocked_processes[i];
            if (temp == NULL) {
                break;
            }
            temp->spent_io_time++;
            if (temp->spent_io_time == temp->io_time) {
                temp->status = READY;
                to_be_enqued[to_be_enqued_count++] = temp;
                to_be_enqued[to_be_enqued_count] = NULL;
                
                // swap it with the last blocked process in list
                blocked_processes[i] = blocked_processes[blocked_count - 1];
                blocked_processes[blocked_count - 1] = NULL;
                blocked_count--;
            } else {
                i++;
            }
        }
        
        /* check if process is running increase its CPU time */
        if (running != NULL) {
            running->spent_cpu_time++;
            if (running->spent_cpu_time == running->cpu_time ||
                running->spent_cpu_time == running->cpu_time * 2) {
                // if * 2 then it should be terminated
                if (running->spent_cpu_time == running->cpu_time * 2) {
                    running->status = TERMINATED;
                    running->turnaround = tick - running->turnaround;
                    terminated_count++;
                    running = NULL;
                } else if (running->io_time != 0) {
                    // it should be io blocked
                    running->status = BLOCKING;
                    blocked_processes[blocked_count++] = running;
                    blocked_processes[blocked_count] = NULL;
                    running = NULL;
                }
            }
        }
        
        
        // sort by process id and enque all
        sort_process_list_by_id(to_be_enqued, to_be_enqued_count);
        build_process_queue(&q, to_be_enqued);
        
        // choose a process to run
        if (running == NULL) {
            running = deque(&q);
            if (running != NULL) {
                running->status = RUNNING;
            } else {
                not_utilized_count++;
            }
        }
        
        // print processes info
        Process *current_process = *process_list;
        int count = 0;
        fprintf(f, "%d: ",tick);
        while(current_process != NULL) {
            switch (current_process->status) {
                case 0:
                    fprintf(f, "%d: running ",current_process->process_id);
                    break;
                case 2:
                    fprintf(f, "%d: blocked ",current_process->process_id);
                    break;
                case 1:
                    fprintf(f, "%d: ready ",current_process->process_id);
                    break;
                default:
                    break;
            }
            current_process = process_list[++count];
        }
        fprintf(f, "\n");
        tick++;
    }
    tick -= 2;
    not_utilized_count -= 1;
    fprintf(f, "Finishing Time: %d\n", tick);
    if (DEBUG_MODE) {
        printf("%d\n", not_utilized_count);
    }
    fprintf(f, "CPU Utilization: %f\n", ((tick - not_utilized_count) * 1.0) / tick);
    
    // print processes info
    Process *current_process = *process_list;
    int count = 0;
    while(current_process != NULL) {
        fprintf(f, "Turnaround process %d: %d\n", current_process->process_id,
                current_process->turnaround);
        current_process = process_list[++count];
    }
    
    return 0;
}


int rrr(Process **process_list, int no_of_processes, int quantum){
    int numProcessesFinished = 0, currentProcessRunTime = 0;
    ProcessQueue queue;
    queue.head = queue.tail = NULL;
    ProcessQueue dummy;
    ProcessQueue ioQueue;
    dummy.head = dummy.tail = NULL;
    int shouldIncrementIO = 1;
    ioQueue.head = ioQueue.tail = NULL;
    Process *currentProcess = NULL;
    int cpuTick=0;
    while(numProcessesFinished < no_of_processes){
        //didn't handle process id rubbish
        int enquedIndex = 0;
        while(enquedIndex < no_of_processes ){
            if(process_list[enquedIndex]->arrival_time == cpuTick){
            Process *p = process_list[enquedIndex++];
            p->status = 1;
                p->executionStatus = 0;
            enque(&queue, p);
            }
            enquedIndex++;
        }
        

        //increment IO
        if(shouldIncrementIO ) {
        while(ioQueue.head != NULL && peak(&ioQueue) != NULL){
            Process* current = deque(&ioQueue);
            current->spent_io_time++;
            if(current->spent_io_time >= current->io_time){
                current->executionStatus = 2;
                current->status = 1;
                current->spent_cpu_time = 0;
                enque(&queue, current);
            } else
                enque(&dummy, current);
        }
        while(dummy.head != NULL && peak(&dummy) != NULL)
            enque(&ioQueue, deque(&dummy));
        }
        shouldIncrementIO = 1;
        if(currentProcess == NULL)
            currentProcess = deque(&queue);
        
        if(currentProcessRunTime >= quantum){
            //preempt the process
                enque(&queue, currentProcess);
            currentProcess = deque(&queue);
            currentProcessRunTime = 0;
        }
        if(currentProcess == NULL){
            int count = 0;
            Process *current_process = *process_list;
            while(current_process != NULL) {
                switch (current_process->status) {
                    case 0:
                        printf("%d | %d: running ",cpuTick,current_process->process_id);
                        break;
                    case 2:
                        printf( "%d | %d: blocked ",cpuTick, current_process->process_id);
                        break;
                    case 1:
                        printf( "%d | %d: ready ",cpuTick,current_process->process_id);
                        break;
                    default:
                        break;
                }
                current_process = process_list[++count];
            }
            printf("\n");

            cpuTick++;
            continue;
        }
            switch(currentProcess->status){
                    // 0: Running, 1: Ready, 2: Blocking, 3: Terminated, None: 4
                case 0:
                    if(currentProcess->spent_cpu_time >= currentProcess->cpu_time){
                        if(currentProcess->executionStatus == 2){
                                currentProcess->status = 3;
                                numProcessesFinished++;
                                currentProcessRunTime = 0;
                            currentProcess = deque(&queue);
                            shouldIncrementIO = 0;
                            continue;
                        } else {
                        currentProcess->status = 2;
                            enque(&ioQueue, currentProcess);
                            currentProcess->spent_cpu_time = 0;
                            currentProcess = deque(&queue);
                            shouldIncrementIO = 0;
                            continue; // because didn't do anything this cycle
                        }


                    }
                    if(currentProcess == NULL){
                        int count = 0;
                        Process *current_process = *process_list;
                        while(current_process != NULL) {
                            switch (current_process->status) {
                                case 0:
                                    printf("%d | %d: running ",cpuTick,current_process->process_id);
                                    break;
                                case 2:
                                    printf( "%d | %d: blocked ",cpuTick, current_process->process_id);
                                    break;
                                case 1:
                                    printf( "%d | %d: ready ",cpuTick,current_process->process_id);
                                    break;
                                default:
                                    break;
                            }
                            current_process = process_list[++count];
                        }
                        printf("\n");

                        cpuTick++;
                        continue;
                    }
                    if(currentProcess->status == 0){
                        currentProcess->spent_cpu_time++;

                    }
                    break;
                case 1:
                    if(currentProcess->executionStatus == 0 || currentProcess->executionStatus == 2){
                        currentProcess->status = 0;
                        currentProcess->spent_cpu_time++;
                    } else {
                        currentProcess->status = 2;
                        currentProcess->spent_io_time++;
                        enque(&ioQueue, currentProcess);
                        currentProcess = deque(&queue);
                    }
                    break;
                case 2:
                    if(currentProcess->spent_io_time >= currentProcess->io_time){
                        currentProcess->status = 0;
                        currentProcess->executionStatus = 2;
                        currentProcess->spent_cpu_time = 1;
//                        printf("%d Process(%d) running\n",cpuTick,currentProcess->process_id);
                    } else {
                        currentProcess->spent_io_time++;
//                        printf("%d Process(%d) blocking\n",cpuTick,currentProcess->process_id);
                    }
                    break;
                default:
                    break;
            }
        int count = 0;
        Process *current_process = *process_list;
        while(current_process != NULL) {
            switch (current_process->status) {
                case 0:
                    printf("%d | %d: running ",cpuTick,current_process->process_id);
                    break;
                case 2:
                    printf( "%d | %d: blocked ",cpuTick, current_process->process_id);
                    break;
                case 1:
                    printf( "%d | %d: ready ",cpuTick,current_process->process_id);
                    break;
                default:
                    break;
            }
            current_process = process_list[++count];
        }
        printf("\n");

        
        currentProcessRunTime++;
        cpuTick++;
    }
    return 0;
}
