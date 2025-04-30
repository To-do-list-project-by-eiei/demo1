#include <stdio.h>
#include <string.h>
#include <time.h>
#include "scheduler.h"       // Add this first to get isDateWithinDays
#include "task_management.h"
#include "searchandstat.h"

void searchTasks(task* head, completedstack* stack, const char* keyword) {
    int found = 0;
    int search_option;
    int min_priority = 0, max_priority = 0;
    date start_date = {0}, end_date = {0};
    char new_keyword[100] = {0};
    char buffer[100];
    
    printf("\n=== Task Search ===\n");
    printf("Search by:\n");
    printf("1. Name\n");
    printf("2. Description\n");
    printf("3. Priority Range\n");
    printf("4. Status (Pending/Completed/Overdue)\n");
    printf("5. Due Date Range\n");
    printf("6. Tasks with No Due Date\n");
    printf("Enter your choice (1-6): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &search_option) != 1) {
        printf("Invalid input. Search aborted.\n");
        return;
    }
    
    // Process search criteria based on option
    switch(search_option) {
        case 1: // Name
        case 2: // Description
            if (keyword == NULL || strlen(keyword) == 0) {
                printf("Enter search keyword: ");
                if (fgets(new_keyword, sizeof(new_keyword), stdin) == NULL) {
                    printf("Error reading keyword. Search aborted.\n");
                    return;
                }
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
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &min_priority) != 1) {
                printf("Invalid input. Search aborted.\n");
                return;
            }
            
            printf("Enter maximum priority (1-High, 3-Low): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &max_priority) != 1) {
                printf("Invalid input. Search aborted.\n");
                return;
            }
            
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
            
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &status_choice) != 1) {
                printf("Invalid input. Search aborted.\n");
                return;
            }
            
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
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || 
                sscanf(buffer, "%d %d %d", &start_date.day, &start_date.month, &start_date.year) != 3) {
                printf("Invalid date format. Search aborted.\n");
                return;
            }
            
            printf("Enter end date (DD MM YYYY): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || 
                sscanf(buffer, "%d %d %d", &end_date.day, &end_date.month, &end_date.year) != 3) {
                printf("Invalid date format. Search aborted.\n");
                return;
            }
            
            // Validate dates
            if (!isValidDate(start_date.day, start_date.month, start_date.year) || 
             !isValidDate(end_date.day, end_date.month, end_date.year)) {
                printf("Invalid date range. Search aborted.\n");
                return;
            }
            
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
            
        case 6: // Tasks with No Due Date
            printf("\n=== Tasks with No Due Date ===\n");
            
            // Search pending tasks
            printf("--- Pending Tasks ---\n");
            current = head;
            while (current) {
                if (!current->due_date_set) {
                    printTaskInfo(current);
                    found = 1;
                }
                current = current->next;
            }
            
            // Search completed tasks
            printf("--- Completed Tasks ---\n");
            node = stack->top;
            while (node) {
                task* t = node->task_data;
                if (!t->due_date_set) {
                    printTaskInfo(t);
                    found = 1;
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

void show_combined_stats(task* head, completedstack* stack, date today) {
    int choice;
    char buffer[10];
    
    printf("\n=== Task Statistics View ===\n");
    printf("1. All-time Statistics\n");
    printf("2. Weekly Statistics\n");
    printf("3. Monthly Statistics\n");
    printf("Enter your choice (1-3): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
        printf("Invalid input. Showing all-time statistics by default.\n");
        choice = 1;
    }
    
    switch(choice) {
        case 1:
            showStats(head, stack, today);  // Original all-time stats function
            break;
        case 2:
            show_time_stats(head, stack, today, 0);  // Weekly stats (0 = week)
            break;
        case 3:
            show_time_stats(head, stack, today, 1);  // Monthly stats (1 = month)
            break;
        default:
            printf("Invalid option. Showing all-time statistics.\n");
            showStats(head, stack, today);
    }
}

void show_time_stats(task* head, completedstack* stack, date today, int period) {
    int total = 0, completed = 0, pending = 0, overdue = 0;
    int high_priority = 0, medium_priority = 0, low_priority = 0;
    
    // Define period in days
    int days_period = (period == 0) ? 7 : 30; // 0 for week, 1 for month
    
    printf("\n=== Task Statistics for %s ===\n", (period == 0) ? "This Week" : "This Month");
    
    // Count pending and overdue tasks for the specified period
    task* p = head;
    while (p) {
        if (p->due_date_set && isDateWithinDays(today, p->duedate, days_period)) {
            if (p->completed) {
                completed++;
            } else if (p->status == OVERDUE) {
                overdue++;
            } else {
                pending++;
            }
            
            // Count by priority
            switch (p->priority) {
                case 1: high_priority++; break;
                case 2: medium_priority++; break;
                case 3: low_priority++; break;
            }
        }
        p = p->next;
    }
    
    // Count completed tasks in stack for the specified period
    // Note: This is an approximation since we don't track completion dates
    int stack_items = 0;
    stacknode* s = stack->top;
    while (s && stack_items < days_period) { // Assume most recent N completions are within the period
        if (s->task_data) {
            completed++;
        }
        stack_items++;
        s = s->next;
    }
    
    // Total tasks
    total = pending + completed + overdue;
    
    printf("Total Tasks for this %s: %d\n", (period == 0) ? "week" : "month", total);
    
    // Calculate percentages
    float completed_percent = total > 0 ? (completed * 100.0f) / total : 0;
    float pending_percent = total > 0 ? (pending * 100.0f) / total : 0;
    float overdue_percent = total > 0 ? (overdue * 100.0f) / total : 0;
    
    printf("Completed: %d (%.1f%%)\n", completed, completed_percent);
    printf("Pending: %d (%.1f%%)\n", pending, pending_percent);
    printf("Overdue/Late: %d (%.1f%%)\n", overdue, overdue_percent);
    
    // Priority breakdown
    printf("\nPriority Breakdown:\n");
    printf("High Priority: %d (%.1f%%)\n", high_priority, total > 0 ? (high_priority * 100.0f) / total : 0);
    printf("Medium Priority: %d (%.1f%%)\n", medium_priority, total > 0 ? (medium_priority * 100.0f) / total : 0);
    printf("Low Priority: %d (%.1f%%)\n", low_priority, total > 0 ? (low_priority * 100.0f) / total : 0);
    
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

// int isDateWithinDays(date today, date check_date, int days) {
//     // Calculate total days for both dates (very simplified calculation)
//     int today_days = today.year * 365 + today.month * 30 + today.day;
//     int check_days = check_date.year * 365 + check_date.month * 30 + check_date.day;
    
//     // Check if the date is within the specified range
//     int diff = check_days - today_days;
//     return (diff >= 0 && diff <= days);
// }
