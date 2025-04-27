#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "scheduler.h"
#include "task_management.h"

int compareDates(date d1, date d2) {
    if (d1.year != d2.year) return d1.year - d2.year;
    if (d1.month != d2.month) return d1.month - d2.month;
    return d1.day - d2.day;
}

date getToday() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    date today = {t->tm_mday, t->tm_mon + 1, t->tm_year + 1900}; // Date structure
    return today;  // Ensure it returns 'date'
}

void setDueDate(task* t, int day, int month, int year) {
    if (t) {
        t->duedate.day = day;
        t->duedate.month = month;
        t->duedate.year = year;
        t->due_date_set = 1;
    }
}

void simulateDayChange(task* head, date today) {
    printf("\n--- Simulating Day Change ---\n");
    while (head) {
        if (!head->completed && head->due_date_set && compareDates(today, head->duedate) > 0) {
            printf("⚠️ Task overdue: %s (was due %02d/%02d/%04d)\n",
                   head->name, head->duedate.day, head->duedate.month, head->duedate.year);
        }
        head = head->next;
    }
}

void adjustPriority(task* head, date today) {
    while (head) {
        if (!head->completed && head->due_date_set && compareDates(today, head->duedate) > 0 && head->priority != 1) {
            head->priority = 1;
            printf("Priority adjusted to HIGH for overdue task: %s\n", head->name);
        }
        head = head->next;
    }
}
void clearcompletedtask(stacknode** top_ptr) { // Renamed parameter for clarity
    stacknode* current = *top_ptr;
    stacknode* temp;

    if (current == NULL) {
        printf("No completed tasks to clear.\n");
        return;
    }

    printf("Clearing all completed tasks...\n");
    while (current != NULL) {
        temp = current;
        current = current->next;

        // Free the dynamically allocated task data FIRST
        if (temp->task_data) { // Good practice to check
            free(temp->task_data);
        }
        // THEN free the stack node itself
        free(temp);
    }

    *top_ptr = NULL; // Set the stack's top pointer to NULL via the double pointer
    printf("All completed tasks cleared.\n");
}
