#ifndef SEARCHANDSTAT_H
#define SEARCHANDSTAT_H

#include "task_management.h"

void searchTasks(task* head, completedstack* stack, const char* keyword);
void showStats(task* head, completedstack* stack, date today);
void doneToday(tasklist* list, completedstack* stack);

#endif
