#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "scheduler.h"
#include "task_management.h"

int comparedate(date day1, date day2) {
    if (day1.year != day2.year) return day1.year - day2.year;
    if (day1.month != day2.month) return day1.month - day2.month;
    return day1.day - day2.day;
}

date gettoday() {
    time_t now = time(0);
    struct tm *t = localtime(&now);
    date today = { t->tm_mday, t->tm_mon + 1, t->tm_year + 1900 };
    return today;
}

void setduedate(task* Task, int day, int month, int year) {
    if (Task) {
        Task->duedate.day = day;
        Task->duedate.month = month;
        Task->duedate.year = year;
    }
}

void checkreminder(task* head) {
    date today = gettoday();
    task* curr = head;
    printf("\n--- Upcoming Due Tasks ---\n");
    while(curr) {
        if (!curr->complete && comparedate(today, curr->duedate) > 0) {
            printf("OVERDUE: %s (Was due on %02d/%02d/%04d)\n",
                   curr->name, curr->duedate.day, curr->duedate.month, curr->duedate.year);
        }
        curr = curr->next;
    }
}

void simulatedaychange(task* head, date newdate) {
    task* curr = head;
    printf("\n--- Simulated Day Check ---\n");
    while(curr) {
        if (!curr->complete && comparedate(newdate, curr->duedate) > 0) {
            printf("OVERDUE: %s (Due %02d/%02d/%04d)\n",
                   curr->name, curr->duedate.day, curr->duedate.month, curr->duedate.year);
        }
        curr = curr->next;
    }
}

void adjustpriority(task* head, date today) {
    task* curr = head;
    while(curr) {
        if (!curr->complete && comparedate(today, curr->duedate) > 0 && curr->priority != 1) {
            curr->priority = 1; // Overdue tasks become high priority
        }
        curr = curr->next;
    }
}

void clearcompletedtask(stacknode** completedstack) {
    while(*completedstack) {
        stacknode* temp = *completedstack;
        *completedstack = (*completedstack)->next;
        free(temp);
    }
    printf("All completed tasks cleared.\n");
}
