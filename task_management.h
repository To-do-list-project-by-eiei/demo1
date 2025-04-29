#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H

#include "task_management.h"

#include "scheduler.h" // Only if needed (but not required if scheduler.h already includes this)

// Task status and date already defined in scheduler.h
// (Be careful to avoid circular includes!)

// typedef enum {
//     PENDING,
//     COMPLETED,
//     OVERDUE
// } TaskStatus;

// typedef struct {
//     int day;
//     int month;
//     int year;
// } date;

typedef struct task {
    char name[100];
    char description[300];
    int priority;
    date duedate;        // ✅ Correct spelling (capital D)
    TaskStatus status;
    int due_date_set;    // ✅ Add this field
    int completed;       // ✅ Add this field
    struct task* next;
} task;

typedef struct {
    task* head;
} tasklist;

typedef struct stacknode {
    task* task_data;  // pointer to the task
    struct stacknode* next;
} stacknode;

typedef struct {
    stacknode* top;
} completedstack;



typedef struct queuenode {
    task* task_data;
    struct queuenode* next;
} queuenode;

typedef struct {
    queuenode* front;
    queuenode* rear;
} taskqueue;

void initQueue(taskqueue* q);
void enqueue(taskqueue* q, task* t);
task* dequeue(taskqueue* q);
int isQueueEmpty(taskqueue* q);
void freeQueue(taskqueue* q);


// Function prototypes
void add(tasklist* list);
int isTaskNameDuplicate(tasklist* list, const char* name);
void view(tasklist* list, date today);
void edit(tasklist* list, const char* name);
void complete(tasklist* list, completedstack* stack, const char* name);
void undoCompleted(tasklist* list, completedstack* stack);
void deleteTask(tasklist* list, const char* name);
void freeTasks(tasklist* list);
void freeStack(completedstack* stack);

#endif // TASK_MANAGEMENT_H
