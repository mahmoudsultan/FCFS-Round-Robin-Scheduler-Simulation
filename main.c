/**
 * Schedulers Simulation OS Lab Assignment 4
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESS_COUNT 10
#define DEBUG_MODE 0

//#include "process.h"
#include "fcfs.h"

int main(int argc, char* argv[]) {

    // extract running arguments
    if (argc != 4) {
        printf("Invalid executing arguments");
        return 0;
    }
    int alg_type = atoi(argv[1]);
    int quantum_time = atoi(argv[2]);
    char* file_name = argv[3];

    if (DEBUG_MODE) {
        printf("%d %d %s \n", alg_type, quantum_time, file_name);
    }

    // read processes data from the input file
    FILE *input_file = fopen(file_name, "r");
    if (input_file == NULL) {
        printf("Processes Info File Not Found");
        return 0;
    }

    Process **process_list = malloc(sizeof(Process) * MAX_PROCESS_COUNT);
    Process *new_process;
    int no_of_processes = 0;
    int process_id, cpu_time, io_time, arrival_time;

    while((fscanf(input_file, "%d %d %d %d", &process_id, &cpu_time, &io_time, &arrival_time)) == 4) {
        new_process = malloc(sizeof(Process));
        new_process->process_id = process_id;
        new_process->cpu_time = cpu_time;
        new_process->io_time = io_time;
        new_process->arrival_time = arrival_time;
        new_process->status = 4;

        if (DEBUG_MODE) {
            printf("DEBUG: Process: %d %d %d %d\n", process_id, cpu_time, io_time, arrival_time);
            printf("DEBUG: struct Process: %d %d %d %d\n", new_process->process_id, new_process->cpu_time,
                   new_process->io_time, new_process->arrival_time);
        }

        process_list[no_of_processes++] = new_process;
    }

    // reallocating to avoid wasted space
    process_list = realloc(process_list, no_of_processes * sizeof(Process));
    process_list[no_of_processes] = NULL;
    if (DEBUG_MODE) {
        printf("DEBUG: No. of Process %d\n", no_of_processes);
        // print the list of process ids after reading
        Process *current_process = *process_list;
        int count = 0;
        while(current_process != NULL) {
            printf("Debug: Process ID: %d\n", current_process->process_id);
            current_process = process_list[++count];
        }
    }

    // run the scheduler alg. based on the argument given
    if (alg_type == 0) {
        sort_process_list(process_list, no_of_processes);
        run_fcfs(process_list, no_of_processes);
    }

    // close the input file
    fclose(input_file);

    // ...
    return 0;
}