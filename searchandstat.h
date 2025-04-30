#ifndef SEARCHANDSTAT_H
#define SEARCHANDSTAT_H

// Include task_management.h directly instead of forward declarations
#include "task_management.h"

// Function declarations
void searchTasks(task* head, completedstack* stack, const char* keyword);
void showStats(task* head, completedstack* stack, date today);
void show_time_stats(task* head, completedstack* stack, date today, int period);
void doneToday(tasklist* list, completedstack* stack);
void printTaskInfo(task* t);

// Add the combined stats function declaration here
void show_combined_stats(task* head, completedstack* stack, date today);

#endif // SEARCHANDSTAT_H
