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
            printf("DEBUG: FCFS: ENQUEUE: node id %d \n", ptr->process->process_id);
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