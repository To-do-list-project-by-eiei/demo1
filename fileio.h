#ifndef FILEIO_H
#define FILEIO_H

#include "task_management.h"

void exportTasks(task* head, const char *filename);
void importTasks(tasklist *list, const char *filename); // only import uses tasklist

#endif
