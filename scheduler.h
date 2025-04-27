#ifndef SCHEDULER_H
#define SCHEDULER_H

//Forward declarations only
typedef struct task task;
typedef struct stacknode stacknode;

// Enums and structs
typedef enum {
    PENDING,
    COMPLETED,
    OVERDUE
} TaskStatus;

typedef struct {
    int day;
    int month;
    int year;
} date;

// Function prototypes
int compareDates(date d1, date d2);
date getToday();
void setDueDate(task* t, int day, int month, int year);
void simulateDayChange(task* head, date today);
void adjustPriority(task* head, date today);
void clearcompletedtask(stacknode** completedstack);

#endif // SCHEDULER_H
