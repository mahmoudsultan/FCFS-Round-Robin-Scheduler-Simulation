//
// Created by mahmoudsultan on 12/14/17.
//

#ifndef SCHEDULERS_FCFS_H
#define SCHEDULERS_FCFS_H

#endif //SCHEDULERS_FCFS_H
#include "process.h"

typedef struct ProcessQueueNode {
    Process *process;
    struct ProcessQueueNode *next;
} ProcessQueueNode;

typedef struct ProcessQueue {
    ProcessQueueNode *head;
    ProcessQueueNode *tail;
} ProcessQueue;

int run_fcfs(Process **process_list, int no_of_processes);