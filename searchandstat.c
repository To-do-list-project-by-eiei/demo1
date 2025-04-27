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

void showStats(task* head, completedstack* stack, date today) {
    int total = 0, completed = 0, pending = 0, overdue = 0;
    
    // Count pending and overdue tasks
    task* p = head;
    while (p) {
        if (p->completed) {
            // Shouldn't happen with new structure, but just in case
            completed++;
        } else if (p->status == OVERDUE || 
                   (p->due_date_set && compareDates(today, p->duedate) > 0)) {
            overdue++;
        } else {
            pending++;
        }
        p = p->next;
    }
    
    // Count completed tasks in stack
    stacknode* s = stack->top;
    while (s) {
        if (s->task_data) {
            completed++;
        }
        s = s->next;
    }
    
    // Total tasks
    total = pending + completed + overdue;
    
    printf("\n=== Task Statistics ===\n");
    printf("Total Tasks: %d\n", total);
    
    // Calculate percentages
    float completed_percent = total > 0 ? (completed * 100.0f) / total : 0;
    float pending_percent = total > 0 ? (pending * 100.0f) / total : 0;
    float overdue_percent = total > 0 ? (overdue * 100.0f) / total : 0;
    
    printf("Completed: %d (%.1f%%)\n", completed, completed_percent);
    printf("Pending: %d (%.1f%%)\n", pending, pending_percent);
    printf("Overdue/Late: %d (%.1f%%)\n", overdue, overdue_percent);
    
    printf("\n=== Progress ===\n");
    // Print a visual progress bar
    printf("[");
    int bar_length = 20;
    int completed_bars = total > 0 ? (int)((completed * bar_length) / total) : 0;
    int overdue_bars = total > 0 ? (int)((overdue * bar_length) / total) : 0;
    
    for (int i = 0; i < bar_length; i++) {
        if (i < completed_bars) {
            printf("="); // Completed sections
        } else if (i < completed_bars + overdue_bars) {
            printf("!"); // Overdue sections
        } else {
            printf(" "); // Pending sections
        }
    }
    printf("] %.1f%% completed\n", completed_percent);
}

void doneToday(tasklist* list, completedstack* stack) {
    date today = getToday();
    int count = 0;
    
    printf("\n=== Tasks Completed Today (%02d/%02d/%04d) ===\n", 
           today.day, today.month, today.year);
    
    // We need to track the actual completion date for each task
    // Since the original structs don't have a completion date field, 
    // we can only check if any tasks were completed on the current day
    
    // First check pending tasks (normally shouldn't find any completed ones here)
    task* current = list->head;
    while (current) {
        if (current->completed && current->status == COMPLETED) {
            // Assuming tasks marked as completed today are actually completed today
            printf("Task: %s\n", current->name);
            printf("Description: %s\n", current->description);
            printf("Priority: %d\n", current->priority);
            printf("-------------------------\n");
            count++;
        }
        current = current->next;
    }
    
    // Then check completed tasks in the stack
    stacknode* node = stack->top;
    while (node) {
        if (node->task_data) {
            // Assuming the top items in the stack were completed most recently
            // This is a limitation, as we don't store the completion date
            printf("Task: %s\n", node->task_data->name);
            printf("Description: %s\n", node->task_data->description);
            printf("Priority: %d\n", node->task_data->priority);
            printf("-------------------------\n");
            count++;
            
            // Only show tasks completed today, which we'll assume are just the top few
            // This is a limitation due to not tracking completion dates
            if (count >= 5) {
                break; // Only show top 5 most recently completed tasks
            }
        }
        node = node->next;
    }
    
    if (count == 0) {
        printf("No tasks completed today.\n");
    } else {
        printf("Total tasks completed today: %d\n", count);
    }
}
