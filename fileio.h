#ifndef FILEIO_H
#define FILEIO_H

#include "task_management.h"

void exportTasksTxt(task* head, completedstack* stack, const char* filename);
void importTasks(tasklist *list, const char *filename);

#endif
