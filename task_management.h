#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H

#include "scheduler.h"

typedef struct task {
    char name[100];
    char description[256];
    int priority;
    int complete;
    date duedate;
    struct task* next;
} task;

typedef struct {
    task* head;
} tasklist;

typedef struct stacknode {
    task task;
    struct stacknode* next;
} stacknode;

typedef struct {
    stacknode* top;
} completedstack;

void add(tasklist* list);
void view(tasklist* list);
void edit(tasklist* list, const char* taskname);
void complete(tasklist* list, completedstack* stack, const char* taskname);
void undocompleted(tasklist* list, completedstack* stack);
void deletetask(tasklist* list, const char* taskname);
void sort(tasklist* list, int criteria);
void progress(tasklist* list, completedstack* stack);

#endif
