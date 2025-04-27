#ifndef SEARCHANDSTAT_H
#define SEARCHANDSTAT_H

#include "task_management.h"

void searchTasks(task* head, const char* keyword);
void showStats(task* head, completedstack* stack);
void doneToday(task* head);

#endif
