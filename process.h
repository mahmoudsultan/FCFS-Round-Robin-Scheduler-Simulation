//
// Created by mahmoudsultan on 12/14/17.
//

#ifndef SCHEDULERS_PROCESS_H
#define SCHEDULERS_PROCESS_H

#endif //SCHEDULERS_PROCESS_H

enum Status {RUNNING, READY, BLOCKING, TERMINATED, NONE};

typedef struct Process {
    int status; // 0: Running, 1: Ready, 2: Blocking, 3: Terminated, None: 4
    int process_id;
    int cpu_time;
    int io_time;
    int arrival_time;
    // for the scheduler program
    int spent_cpu_time;
    int spent_io_time;
    int turnaround;
} Process;

void sort_process_list(Process** process_list, int no_of_processes);
void sort_process_list_by_id(Process** process_list, int no_of_processes);