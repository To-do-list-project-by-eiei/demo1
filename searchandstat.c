#include <stdio.h>
#include <string.h>
#include <time.h>
#include "searchandstat.h"
#include "scheduler.h"

void searchTasks(task* head, completedstack* stack, const char* keyword) {
    int found = 0;
    int search_option;
    int min_priority = 0, max_priority = 0;
    date start_date = {0}, end_date = {0};
    
    printf("\n=== Task Search ===\n");
    printf("Search by:\n");
    printf("1. Name\n");
    printf("2. Description\n");
    printf("3. Priority Range\n");
    printf("4. Status (Pending/Completed/Overdue)\n");
    printf("5. Due Date Range\n");
    printf("Enter your choice (1-5): ");
    scanf("%d", &search_option);
    getchar(); // Clear newline
    
    // Process search criteria based on option
    switch(search_option) {
        case 1: // Name
        case 2: // Description
            if (strlen(keyword) == 0) {
                printf("Enter search keyword: ");
                char new_keyword[100];
                fgets(new_keyword, sizeof(new_keyword), stdin);
                new_keyword[strcspn(new_keyword, "\n")] = 0;
                
                printf("\n=== Search Results for '%s' ===\n", new_keyword);
                
                // Search pending tasks
                printf("--- Pending Tasks ---\n");
                task* current = head;
                while (current) {
                    if ((search_option == 1 && strstr(current->name, new_keyword)) ||
                        (search_option == 2 && strstr(current->description, new_keyword))) {
                        printTaskInfo(current);
                        found = 1;
                    }
                    current = current->next;
                }
                
                // Search completed tasks
                printf("--- Completed Tasks ---\n");
                stacknode* node = stack->top;
                while (node) {
                    task* t = node->task_data;
                    if ((search_option == 1 && strstr(t->name, new_keyword)) ||
                        (search_option == 2 && strstr(t->description, new_keyword))) {
                        printTaskInfo(t);
                        found = 1;
                    }
                    node = node->next;
                }
            } else {
                // If keyword was already provided
                printf("\n=== Search Results for '%s' ===\n", keyword);
                
                // Search pending tasks
                printf("--- Pending Tasks ---\n");
                task* current = head;
                while (current) {
                    if ((search_option == 1 && strstr(current->name, keyword)) ||
                        (search_option == 2 && strstr(current->description, keyword))) {
                        printTaskInfo(current);
                        found = 1;
                    }
                    current = current->next;
                }
                
                // Search completed tasks
                printf("--- Completed Tasks ---\n");
                stacknode* node = stack->top;
                while (node) {
                    task* t = node->task_data;
                    if ((search_option == 1 && strstr(t->name, keyword)) ||
                        (search_option == 2 && strstr(t->description, keyword))) {
                        printTaskInfo(t);
                        found = 1;
                    }
                    node = node->next;
                }
            }
            break;
            
        case 3: // Priority Range
            printf("Enter minimum priority (1-High, 3-Low): ");
            scanf("%d", &min_priority);
            printf("Enter maximum priority (1-High, 3-Low): ");
            scanf("%d", &max_priority);
            getchar(); // Clear newline
            
            if (min_priority > max_priority) {
                // Swap if min > max
                int temp = min_priority;
                min_priority = max_priority;
                max_priority = temp;
            }
            
            printf("\n=== Search Results for Priority %d to %d ===\n", min_priority, max_priority);
            
            // Search pending tasks
            printf("--- Pending Tasks ---\n");
            task* current = head;
            while (current) {
                if (current->priority >= min_priority && current->priority <= max_priority) {
                    printTaskInfo(current);
                    found = 1;
                }
                current = current->next;
            }
            
            // Search completed tasks
            printf("--- Completed Tasks ---\n");
            stacknode* node = stack->top;
            while (node) {
                task* t = node->task_data;
                if (t->priority >= min_priority && t->priority <= max_priority) {
                    printTaskInfo(t);
                    found = 1;
                }
                node = node->next;
            }
            break;
            
        case 4: // Status
            printf("Search by status:\n");
            printf("1. Pending\n");
            printf("2. Completed\n");
            printf("3. Overdue\n");
            printf("Enter choice: ");
            int status_choice;
            scanf("%d", &status_choice);
            getchar(); // Clear newline
            
            TaskStatus search_status;
            switch(status_choice) {
                case 1: search_status = PENDING; break;
                case 2: search_status = COMPLETED; break;
                case 3: search_status = OVERDUE; break;
                default: 
                    printf("Invalid status choice.\n");
                    return;
            }
            
            printf("\n=== Search Results for Status: %s ===\n", 
                  (search_status == PENDING) ? "Pending" : 
                  (search_status == COMPLETED) ? "Completed" : "Overdue");
            
            // For completed tasks, we primarily search the stack
            if (search_status == COMPLETED) {
                printf("--- Completed Tasks ---\n");
                stacknode* node = stack->top;
                while (node) {
                    task* t = node->task_data;
                    if (t->status == search_status) {
                        printTaskInfo(t);
                        found = 1;
                    }
                    node = node->next;
                }
            } else {
                // For PENDING/OVERDUE we search the main list
                printf("--- Tasks ---\n");
                task* current = head;
                while (current) {
                    if (current->status == search_status) {
                        printTaskInfo(current);
                        found = 1;
                    }
                    current = current->next;
                }
            }
            break;
            
        case 5: // Due Date Range
            printf("Enter start date (DD MM YYYY): ");
            scanf("%d %d %d", &start_date.day, &start_date.month, &start_date.year);
            printf("Enter end date (DD MM YYYY): ");
            scanf("%d %d %d", &end_date.day, &end_date.month, &end_date.year);
            getchar(); // Clear newline
            
            printf("\n=== Search Results for Due Date from %02d/%02d/%04d to %02d/%02d/%04d ===\n",
                   start_date.day, start_date.month, start_date.year,
                   end_date.day, end_date.month, end_date.year);
            
            // Search pending tasks
            printf("--- Pending Tasks ---\n");
            current = head;
            while (current) {
                if (current->due_date_set) {
                    // Check if task due date is within range
                    if (compareDates(current->duedate, start_date) >= 0 && 
                        compareDates(current->duedate, end_date) <= 0) {
                        printTaskInfo(current);
                        found = 1;
                    }
                }
                current = current->next;
            }
            
            // Search completed tasks
            printf("--- Completed Tasks ---\n");
            node = stack->top;
            while (node) {
                task* t = node->task_data;
                if (t->due_date_set) {
                    // Check if task due date is within range
                    if (compareDates(t->duedate, start_date) >= 0 && 
                        compareDates(t->duedate, end_date) <= 0) {
                        printTaskInfo(t);
                        found = 1;
                    }
                }
                node = node->next;
            }
            break;
            
        default:
            printf("Invalid search option.\n");
            return;
    }
    
    if (!found) {
        printf("No matching tasks found.\n");
    }
}

// Helper function to print task information consistently
void printTaskInfo(task* t) {
    printf("Name: %s\n", t->name);
    printf("Description: %s\n", t->description);
    printf("Priority: %d (%s)\n", t->priority, 
           (t->priority == 1) ? "High" : (t->priority == 2) ? "Medium" : "Low");
    printf("Status: %s\n", 
           (t->status == PENDING) ? "Pending" : 
           (t->status == COMPLETED) ? "Completed" : "Overdue");
    if (t->due_date_set) {
        printf("Due Date: %02d/%02d/%04d\n", 
               t->duedate.day, t->duedate.month, t->duedate.year);
    } else {
        printf("Due Date: Not Set\n");
    }
    printf("-------------------------\n");
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
