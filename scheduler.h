#ifndef SCHEDULER_H
#define SCHEDULER_H


typedef struct task task;
typedef struct stacknode stacknode;


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


int compareDates(date d1, date d2);
date getToday();
void setDueDate(task* t, int day, int month, int year);
void simulateDayChange(task* head, date* currentDate);
void adjustPriority(task* head, date today);
void autoPriorityAdjust(task* head, date today);  
int getDaysBetween(date d1, date d2);  
void clearcompletedtask(stacknode** completedstack);
void updateTaskStatuses(task* head, date today);
int isDateSoon(date today, date duedate, int daysThreshold);

// checking if a date is valid
int isValidDate(int day, int month, int year);

// checking if a date is within a certain number of days
int isDateWithinDays(date today, date check_date, int days);

// reminders
void checkReminders(task* head, date today);

#endif 
