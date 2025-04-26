#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct {
    int day;
    int month;
    int year;
} date;

// Forward declaration
struct task;
struct stacknode;

int comparedate(date day1, date day2);
date gettoday();
void setduedate(struct task* Task, int day, int month, int year);
void checkreminder(struct task* head);
void simulatedaychange(struct task* head, date newdate);
void adjustpriority(struct task* head, date today);
void clearcompletedtask(struct stacknode** completedstack);

#endif
