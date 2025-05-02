#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Add this line to fix strcpy error
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

int isValidDate(int day, int month, int year) {
    if (year < 1900 || month < 1 || month > 12 || day < 1)
        return 0;
        
    // Days in each month
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Adjust for leap years
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        daysInMonth[2] = 29;
        
    return day <= daysInMonth[month];
}

void setDueDate(task* t, int day, int month, int year) {
    if (t) {
        t->duedate.day = day;
        t->duedate.month = month;
        t->duedate.year = year;
        t->due_date_set = 1;
    }
}

// void simulateDayChange(task* head, date today) {
//     printf("\n--- Simulating Day Change ---\n");
//     while (head) {
//         if (!head->completed && head->due_date_set && compareDates(today, head->duedate) > 0) {
//             printf("⚠️ Task overdue: %s (was due %02d/%02d/%04d)\n",
//                    head->name, head->duedate.day, head->duedate.month, head->duedate.year);
//         }
//         head = head->next;
//     }
// }

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

void updateTaskStatuses(task* head, date today) {
    task* current = head;
    while (current) {
        if (!current->completed && current->due_date_set) {
            // Check if task is overdue
            if (compareDates(today, current->duedate) > 0) {
                current->status = OVERDUE;
            } 
            // Check if task is due soon (within 2 days)
            else if (isDateSoon(today, current->duedate, 2)) {
                current->status = PENDING;  // Still pending but will mark as urgent in display
            }
        }
        current = current->next;
    }
}

// Improved auto-priority adjustment based on due dates
void autoPriorityAdjust(task* head, date today) {
    task* current = head;
    while (current) {
        if (!current->completed && current->due_date_set) {
            // Calculate days until due
            int daysUntilDue = getDaysBetween(today, current->duedate);
            
            // Auto-adjust priority based on due date
            if (daysUntilDue < 0) {
                // Overdue tasks are always high priority
                if (current->priority != 1) {
                    current->priority = 1;
                    printf("Priority for '%s' auto-adjusted to HIGH \n", current->name);
                }
            }
            else if (daysUntilDue <= 2) {
                // Tasks due within 2 days are at least medium priority
                if (current->priority > 1) {
                    current->priority = 1;
                    printf("Priority for '%s' auto-adjusted to HIGH \n", current->name);
                }
            }
            else if (daysUntilDue <= 1) {
                // Tasks due tomorrow are high priority
                if (current->priority != 1) {
                    current->priority = 1;
                    printf("Priority for '%s' auto-adjusted to HIGH \n", current->name);
                }
            }
        }
        current = current->next;
    }
}

int getDaysBetween(date d1, date d2) {
    // This is a simplified calculation and doesn't account for all edge cases
    // For a more accurate calculation, convert dates to absolute days or use a library
    
    // Approximate days in each month (ignoring leap years)
    const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Convert dates to days since year 0 (approximate)
    int days1 = d1.year * 365 + d1.day;
    int days2 = d2.year * 365 + d2.day;
    
    // Add days for months
    for (int i = 1; i < d1.month; i++) days1 += daysInMonth[i];
    for (int i = 1; i < d2.month; i++) days2 += daysInMonth[i];
    
    // Add leap years (approximate)
    days1 += d1.year / 4;
    days2 += d2.year / 4;
    
    return days2 - days1;
}


int isDateSoon(date today, date duedate, int daysThreshold) {
    // Simple implementation - not accounting for month/year boundaries
    // For a more accurate implementation, convert both dates to days since epoch
    
    // If years are different
    if (duedate.year > today.year) {
        if (duedate.month == 1 && today.month == 12) {
            // Special case: December -> January transition
            return (31 - today.day + duedate.day) <= daysThreshold;
        }
        return 0; // Not soon if more than a month away
    }
    
    // If within same year but different months
    if (duedate.month > today.month) {
        if (duedate.month - today.month == 1) {
            // Tasks due early next month
            int daysInCurrentMonth;
            switch (today.month) {
                case 2: daysInCurrentMonth = 28; break; // Simplified, ignoring leap years
                case 4: case 6: case 9: case 11: daysInCurrentMonth = 30; break;
                default: daysInCurrentMonth = 31;
            }
            return (daysInCurrentMonth - today.day + duedate.day) <= daysThreshold;
        }
        return 0; // Not soon if more than a month away
    }
    
    // Same month, just compare days
    return (duedate.day - today.day) <= daysThreshold && (duedate.day - today.day) >= 0;
}

// Simplified simulateDayChange function without Unicode
// Table format simulateDayChange function
void simulateDayChange(task* head, date* currentDate) {
    date newDate;
    
    printf("\n=== Simulate Day Change ===\n");
    printf("Current date: %02d/%02d/%04d\n\n", currentDate->day, currentDate->month, currentDate->year);
    printf("Enter new date (DD MM YYYY): ");
    
    char buffer[20];
    
    // Use fgets and sscanf for safer input
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || 
        sscanf(buffer, "%d %d %d", &newDate.day, &newDate.month, &newDate.year) != 3) {
        printf("Invalid date format. Simulation cancelled.\n");
        return;
    }
    
    // Validate date
    if (!isValidDate(newDate.day, newDate.month, newDate.year)) {
        printf("Invalid date. Simulation cancelled.\n");
        return;
    }
    
    // Update the current date
    *currentDate = newDate;
    printf("\nDate changed to: %02d/%02d/%04d\n", currentDate->day, currentDate->month, currentDate->year);
    
    // Update task statuses based on new date
    updateTaskStatuses(head, newDate);
    
    // Auto-adjust priorities based on due dates
    autoPriorityAdjust(head, newDate);
    
    // Create arrays to store tasks by status
    task* overdue_tasks[100];
    task* urgent_tasks[100];
    task* normal_tasks[100];
    int overdue_count = 0;
    int urgent_count = 0;
    int normal_count = 0;
    
    // Categorize tasks
    task* current = head;
    while (current) {
        if (!current->completed && current->due_date_set) {
            if (compareDates(newDate, current->duedate) > 0) {
                overdue_tasks[overdue_count++] = current;
            } 
            else if (isDateSoon(newDate, current->duedate, 2)) {
                urgent_tasks[urgent_count++] = current;
            }
            else {
                normal_tasks[normal_count++] = current;
            }
        }
        current = current->next;
    }
    
    // Display tasks in table format
    printf("\n=== Task Status Overview ===\n");
    printf("%-3s %-25s %-10s %-15s %-10s\n", "#", "Task Name", "Priority", "Due Date", "Status");
    printf("---------------------------------------------------------------\n");
    
    int count = 1;
    
    // First show overdue tasks
    if (overdue_count > 0) {
        printf("\n--- OVERDUE TASKS ---\n");
        for (int i = 0; i < overdue_count; i++) {
            char priority_str[10];
            switch(overdue_tasks[i]->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            printf("%-3d %-25s %-10s %02d/%02d/%04d OVERDUE\n", 
                   count++, 
                   overdue_tasks[i]->name, 
                   priority_str,
                   overdue_tasks[i]->duedate.day, 
                   overdue_tasks[i]->duedate.month, 
                   overdue_tasks[i]->duedate.year);
        }
    }
    
    // Then show urgent tasks
    if (urgent_count > 0) {
        printf("\n--- URGENT TASKS (Due within 2 days) ---\n");
        for (int i = 0; i < urgent_count; i++) {
            char priority_str[10];
            switch(urgent_tasks[i]->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            printf("%-3d %-25s %-10s %02d/%02d/%04d URGENT\n", 
                   count++, 
                   urgent_tasks[i]->name, 
                   priority_str,
                   urgent_tasks[i]->duedate.day, 
                   urgent_tasks[i]->duedate.month, 
                   urgent_tasks[i]->duedate.year);
        }
    }
    
    // Show summary in a clean format
    printf("\n=== Summary ===\n");
    printf("+-----------------------+----------+\n");
    printf("| Status                | Count    |\n");
    printf("+-----------------------+----------+\n");
    printf("| OVERDUE Tasks         | %-8d |\n", overdue_count);
    printf("| URGENT Tasks          | %-8d |\n", urgent_count);
    printf("| Normal Tasks          | %-8d |\n", normal_count);
    printf("+-----------------------+----------+\n");
    printf("| Total Active Tasks    | %-8d |\n", overdue_count + urgent_count + normal_count);
    printf("+-----------------------+----------+\n");
    
    if (overdue_count == 0 && urgent_count == 0) {
        printf("\nNo overdue or urgent tasks found. All tasks are on schedule.\n");
    }
    
    printf("\nDay change simulation completed.\n");
}
// int isValidDate(int day, int month, int year) {
//     if (year < 1900 || month < 1 || month > 12 || day < 1)
//         return 0;
//     int daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
//     if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
//         daysInMonth[2] = 29;
//     return day <= daysInMonth[month];
// }

int isDateWithinDays(date today, date check_date, int days) {
    // Calculate total days for both dates (very simplified calculation)
    int today_days = today.year * 365 + today.month * 30 + today.day;
    int check_days = check_date.year * 365 + check_date.month * 30 + check_date.day;
    
    // Check if the date is within the specified range
    int diff = check_days - today_days;
    return (diff >= 0 && diff <= days);
}
