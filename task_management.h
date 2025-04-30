#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H

// Only include scheduler.h
#include "scheduler.h" // For date and TaskStatus types

// Constants for tags
#define MAX_TAGS 5
#define MAX_TAG_LENGTH 20

// Task structure
typedef struct task {
    char name[100];
    char description[300];
    int priority;
    date duedate;
    TaskStatus status;
    int due_date_set;
    int completed;
    
    // Tag fields
    char tags[MAX_TAGS][MAX_TAG_LENGTH];
    int tag_count;
    
    struct task* next;
} task;

// List and stack structures
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

// Queue structures
typedef struct queuenode {
    task* task_data;
    struct queuenode* next;
} queuenode;

typedef struct {
    queuenode* front;
    queuenode* rear;
} taskqueue;

// Queue function prototypes
void initQueue(taskqueue* q);
void enqueue(taskqueue* q, task* t);
task* dequeue(taskqueue* q);
int isQueueEmpty(taskqueue* q);
void freeQueue(taskqueue* q);

// Basic task management function prototypes
void sortTasksByDueDate(task* tasks[], int count);
void add(tasklist* list);
int isTaskNameDuplicate(tasklist* list, const char* name);
void view(tasklist* list, date today);
void edit(tasklist* list, const char* name);
void complete(tasklist* list, completedstack* stack, const char* name);
void undoCompleted(tasklist* list, completedstack* stack);
void deleteTask(tasklist* list, const char* name);
void freeTasks(tasklist* list);
void freeStack(completedstack* stack);

// New feature function prototypes
void simplified_view(tasklist* list, date today);
void view_weekly_summary(tasklist* list, date today);
void view_monthly_summary(tasklist* list, date today);
void add_tag_to_task(tasklist* list, const char* taskname);
void view_by_tag(tasklist* list, const char* tag);
void sort_by_tag(tasklist* list);
void text_converter(const char* input_text, tasklist* list);

// Combined view functions
void view_combined(tasklist* list, date today);
void view_time_summary(tasklist* list, date today);

// Do NOT declare functions from searchandstat.h here

#endif // TASK_MANAGEMENT_H
