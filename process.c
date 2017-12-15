#include "process.h"
#include <stdio.h>

void sort_process_list(Process** process_list, int no_of_processes) {
    int count = 0;
    int swapped = 1;
    Process* temp;
    int sorted = 0;

    while(swapped == 1)
    {
        swapped = 0;

        for(count = sorted; count < no_of_processes - sorted - 1; count++) {
            if (process_list[count]->arrival_time > process_list[count + 1]->arrival_time) {
                //sorting by arrival time
                temp = process_list[count];
                process_list[count] = process_list[count + 1];
                process_list[count + 1] = temp;
                swapped = 1;
            }

            if (process_list[count]->arrival_time == process_list[count + 1]->arrival_time &&
                process_list[count]->process_id > process_list[count + 1]->process_id) {
                //sorting process id
                temp = process_list[count];
                process_list[count] = process_list[count + 1];
                process_list[count + 1] = temp;
                swapped = 1;
            }
        }
        sorted++;
    }
}

void sort_process_list_by_id(Process** process_list, int no_of_processes) {
    int count = 0;
    int swapped = 1;
    Process* temp;
    int sorted = 0;

    while(swapped == 1)
    {
        swapped = 0;

        for(count = sorted; count < no_of_processes - sorted - 1; count++) {
            if (process_list[count+1] != NULL && process_list[count]->process_id > process_list[count + 1]->process_id) {
                //sorting by arrival time
                temp = process_list[count];
                process_list[count] = process_list[count + 1];
                process_list[count + 1] = temp;
                swapped = 1;
            }
        }
        sorted++;
    }
}