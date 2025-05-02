#ifndef SEARCHANDSTAT_H
#define SEARCHANDSTAT_H


#include "task_management.h"


void searchTasks(task* head, completedstack* stack, const char* keyword);
void showStats(task* head, completedstack* stack, date today);
void show_time_stats(task* head, completedstack* stack, date today, int period);
void doneToday(tasklist* list, completedstack* stack);
void printTaskInfo(task* t);


void show_combined_stats(task* head, completedstack* stack, date today);

#endif 
