#ifndef SCHEDULER_H
#define SCHEDULER_H

// Forward declarations only
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
void simulateDayChange(task* head, date* currentDate);
void adjustPriority(task* head, date today);
void autoPriorityAdjust(task* head, date today);  // New function
int getDaysBetween(date d1, date d2);  // New function
void clearcompletedtask(stacknode** completedstack);
void updateTaskStatuses(task* head, date today);
int isDateSoon(date today, date duedate, int daysThreshold);

// New function for reminders
void checkReminders(task* head, date today);

#endif // SCHEDULER_H
