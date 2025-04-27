#include <stdio.h>
#include <string.h>
#include <time.h>
#include "searchandstat.h"
#include "scheduler.h"

void searchTasks(task* head, const char* keyword) {
    int found = 0;
    while (head) {
        if (strstr(head->name, keyword) || strstr(head->description, keyword)) {
            printf("Found: %s | Priority: %d | Status: %s\n",
                   head->name, head->priority, head->completed ? "Completed" : "Pending");
            found = 1;
        }
        head = head->next;
    }
    if (!found) printf("No task matching '%s' found.\n", keyword);
}

void showStats(task* head, completedstack* stack) {
    int total = 0, completed = 0;
    task* p = head;
    while (p) {
        total++;
        p = p->next;
    }
    stacknode* s = stack->top;
    while (s) {
        completed++;
        s = s->next;
    }
    printf("\n=== Task Statistics ===\n");
    printf("Total Tasks: %d\n", total + completed);
    printf("Completed: %d\n", completed);
    printf("Pending: %d\n", total);
}

void doneToday(task* head) {
    date today = getToday();
    int count = 0;
    while (head) {
        if (head->completed && head->due_date_set &&
            head->duedate.day == today.day &&
            head->duedate.month == today.month &&
            head->duedate.year == today.year) {
            printf("Task completed today: %s\n", head->name);
            count++;
        }
        head = head->next;
    }
    if (count == 0)
        printf("No tasks completed today.\n");
}
