#ifndef SEARCHANDSTAT_H
#define SEARCHANDSTAT_H

#include "task_management.h"

void search(task* head, const char* keyword);
void exportTasks(task* head, const char* filename);
void stat(task* head);
void doneToday(task* head);

#endif
