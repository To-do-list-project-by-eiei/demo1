#include <stdio.h>
#include <string.h>
#include <time.h>
#include "searchandstat.h"
#include "scheduler.h"

void searchTasks(task* head, completedstack* stack, const char* keyword) {
    int found = 0;
    
    printf("\n=== Search Results for '%s' ===\n", keyword);
    
    // Search pending tasks
    printf("--- Pending Tasks ---\n");
    task* current = head;
    while (current) {
        if (strstr(current->name, keyword) || strstr(current->description, keyword)) {
            printf("Name: %s\n", current->name);
            printf("Description: %s\n", current->description);
            printf("Priority: %d\n", current->priority);
            if (current->due_date_set) {
                printf("Due Date: %02d/%02d/%04d\n", 
                       current->duedate.day, current->duedate.month, current->duedate.year);
            }
            printf("-------------------------\n");
            found = 1;
        }
        current = current->next;
    }
    
    // Search completed tasks
    printf("--- Completed Tasks ---\n");
    stacknode* node = stack->top;
    while (node) {
        task* t = node->task_data;
        if (strstr(t->name, keyword) || strstr(t->description, keyword)) {
            printf("Name: %s\n", t->name);
            printf("Description: %s\n", t->description);
            printf("Priority: %d\n", t->priority);
            if (t->due_date_set) {
                printf("Due Date: %02d/%02d/%04d\n", 
                       t->duedate.day, t->duedate.month, t->duedate.year);
            }
            printf("-------------------------\n");
            found = 1;
        }
        node = node->next;
    }
    
    if (!found) {
        printf("No task matching '%s' found.\n", keyword);
    }
}

void showStats(task* head, completedstack* stack) {
    int total = 0, completed = 0, pending = 0, late = 0;
    date today = getToday();
    
    // Count pending and late tasks
    task* p = head;
    while (p) {
        pending++;
        if (!p->completed && p->due_date_set && compareDates(today, p->duedate) > 0) {
            late++;
        }
        p = p->next;
    }
    
    // Count completed tasks
    stacknode* s = stack->top;
    while (s) {
        completed++;
        s = s->next;
    }
    
    // Total tasks
    total = pending + completed;
    
    printf("\n=== Task Statistics ===\n");
    printf("Total Tasks: %d\n", total);
    
    // Calculate percentages
    float completed_percent = total > 0 ? (completed * 100.0f) / total : 0;
    float pending_percent = total > 0 ? (pending * 100.0f) / total : 0;
    float late_percent = total > 0 ? (late * 100.0f) / total : 0;
    
    printf("Completed: %d (%.1f%%)\n", completed, completed_percent);
    printf("Pending: %d (%.1f%%)\n", pending, pending_percent);
    printf("Late: %d (%.1f%%)\n", late, late_percent);
    
    printf("\n=== Progress ===\n");
    // Print a simple visual progress bar
    printf("[");
    int bar_length = 20;
    int completed_bars = total > 0 ? (int)((completed * bar_length) / total) : 0;
    
    for (int i = 0; i < bar_length; i++) {
        if (i < completed_bars) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %.1f%%\n", completed_percent);
}

void doneToday(tasklist* list, completedstack* stack) {
    date today = getToday();
    int count = 0;
    
    printf("\n=== Tasks Completed Today ===\n");
    
    // Check for any completed tasks still in the main list
    // (Though normally completed tasks should be in the stack)
    task* current = list->head;
    while (current) {
        if (current->completed) {
            printf("Task in main list: %s\n", current->name);
            printf("Description: %s\n", current->description);
            printf("-------------------------\n");
            count++;
        }
        current = current->next;
    }
    
    // The main place to find completed tasks - the stack
    stacknode* node = stack->top;
    while (node) {
        if (node->task_data) {  // Make sure task_data is not NULL
            printf("Completed task: %s\n", node->task_data->name);
            printf("Description: %s\n", node->task_data->description);
            printf("-------------------------\n");
            count++;
        }
        node = node->next;
    }
    
    if (count == 0) {
        printf("No tasks have been completed today.\n");
    } else {
        printf("Total tasks completed today: %d\n", count);
    }
}

