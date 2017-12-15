/**
 * FCFS Scheduler Algorithm Simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include "fcfs.h"

#define DEBUG_MODE 0

void enque(ProcessQueue *q, Process *process) {
    if (q == NULL) {
        fprintf(stderr, "Queue is Null");
        return;
    }

    ProcessQueueNode *processQueueNode = malloc(sizeof(ProcessQueueNode));
    processQueueNode->process = process;

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
            printf("DEBUG: FCFS: QUEUE AFTER DEQUE: node id %d \n", ptr->process->process_id);
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
            printf("DEBUG: FCFS: QUEUE: node id %d \n", ptr->process->process_id);
            ptr = ptr->next;
        }
    }
}

int run_fcfs(Process **process_list, int no_of_processes) {
    // keep a queue of ready processes
    ProcessQueue q;
    q.head = q.tail = NULL;
    build_process_queue(&q, process_list);
    int ready_count = no_of_processes;

    // keep a list of IO Blocked processes
    Process** blocked_processes = malloc(sizeof(Process) * (no_of_processes - 1));
    int blocked_count = 0;

    // keep track of running process
    Process* running = NULL;


    int i;
    Process* temp;
    // start cpu ticking
    // while there's a process either in ready queue or blocked queue
    int tick = 0;

    while(ready_count || blocked_count) {

        // handling running processes
        if (running == NULL) {
            running = peak(&q);
            // check if the next process arrived or not
            if (running->arrival_time > tick) {
                running->status = 4;
                running = NULL;
            } else {
                deque(&q);
                running->status = 0;
                ready_count--;
            }
        } else {
            // increase the CPU time of the running process
            // check if process finished it's cpu time block it and choose another
            // process to run
            running->spent_cpu_time += 1;
            if (running->spent_cpu_time == running->cpu_time || running->spent_cpu_time == running->cpu_time * 2) {
                // process finished CPU time block it or remove it from system
                if (running->spent_cpu_time == running->cpu_time) {
                    // block it
                    running->status = 2;
                    blocked_processes[blocked_count] = running;
                    blocked_count++;
                } else {
                    // terminate process
                    running->status = 3;
                }

                // choose new process
                running = peak(&q);
                // check if the next process arrived or not
                if (running->arrival_time > tick) {
                    running->status = 4;
                    running = NULL;
                } else {
                    deque(&q);
                    running->status = 0;
                    ready_count--;
                }
            }
        }

        // handling blocked processes
        i = 0;
        while (i < blocked_count) {
            temp = blocked_processes[i];
            temp->spent_io_time++;

            if (temp->spent_io_time == temp->io_time) {
                // enque to the ready queue and increment ready count
                enque(&q, temp);
                temp->status = 1;
                ready_count++;

                // swap with the last blocked process to avoid null places
                blocked_processes[i] = blocked_processes[blocked_count - 1];
                blocked_processes[blocked_count - 1] = NULL;
                blocked_count--;
            } else {
                i++;
            }
        }

        // print processes info
        Process *current_process = *process_list;
        int count = 0;
        printf("%d: ",tick);
        while(current_process != NULL) {
            switch (current_process->status) {
                case 0:
                    printf("%d: running ",current_process->process_id);
                    break;
                case 2:
                    printf("%d: blocked ",current_process->process_id);
                    break;
                case 1:
                    printf("%d: ready ",current_process->process_id);
                    break;
                default:
                    break;
            }
            current_process = process_list[++count];
        }
        printf("\n");
        tick++;
    }
    return 0;
}