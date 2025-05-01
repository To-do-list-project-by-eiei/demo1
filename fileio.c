#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // For isspace()
#include "fileio.h"
#include "scheduler.h"  // For isValidDate()

// Update the exportTasksTxt function in fileio.c to export all tasks organized by priority

void exportTasksTxt(task* head, completedstack* stack, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for export");
        return;
    }

    // Get current date for export timestamp
    date today = getToday();
    
    // Track how many tasks we export in each category
    int pending_count = 0, completed_count = 0, overdue_count = 0;
    int high_count = 0, medium_count = 0, low_count = 0;
    int total_exported = 0;

    // Write a header to the file
    fprintf(file, "===== TO-DO LIST EXPORT =====\n");
    fprintf(file, "Date Exported: %02d/%02d/%04d\n\n", today.day, today.month, today.year);

    // Count tasks in each category for summary
    task* current = head;
    while (current) {
        if (!current->completed) {
            if (current->status == OVERDUE) {
                overdue_count++;
            } else {
                pending_count++;
            }
            
            // Count by priority
            switch(current->priority) {
                case 1: high_count++; break;
                case 2: medium_count++; break;
                case 3: low_count++; break;
            }
        }
        current = current->next;
    }
    
    // Count completed tasks
    stacknode* node = stack->top;
    while (node) {
        completed_count++;
        node = node->next;
    }
    
    // Write summary at the top (like simplified view)
    fprintf(file, "SUMMARY: Overdue: %d | Pending: %d | Completed: %d\n", 
            overdue_count, pending_count, completed_count);
    fprintf(file, "PRIORITIES: High: %d | Medium: %d | Low: %d\n\n", 
            high_count, medium_count, low_count);
    
    // Create arrays for tasks by status and priority for sorting
    task* overdue_tasks[200];  // For overdue tasks
    task* priority_tasks[3][200]; // For pending tasks by priority (1-3)
    int overdue_tasks_count = 0;
    int priority_tasks_count[3] = {0};
    
    // Collect tasks by status and priority
    current = head;
    while (current) {
        if (!current->completed) {
            if (current->status == OVERDUE) {
                overdue_tasks[overdue_tasks_count++] = current;
            } else {
                int priority_idx = current->priority - 1;
                if (priority_idx >= 0 && priority_idx < 3) {
                    priority_tasks[priority_idx][priority_tasks_count[priority_idx]++] = current;
                }
            }
        }
        current = current->next;
    }
    
    // Sort overdue tasks by due date
    for (int i = 0; i < overdue_tasks_count - 1; i++) {
        for (int j = 0; j < overdue_tasks_count - i - 1; j++) {
            if (overdue_tasks[j]->due_date_set && overdue_tasks[j+1]->due_date_set) {
                if (compareDates(overdue_tasks[j]->duedate, overdue_tasks[j+1]->duedate) > 0) {
                    // Swap
                    task* temp = overdue_tasks[j];
                    overdue_tasks[j] = overdue_tasks[j+1];
                    overdue_tasks[j+1] = temp;
                }
            }
        }
    }
    
    // Sort each priority group by due date
    for (int p = 0; p < 3; p++) {
        for (int i = 0; i < priority_tasks_count[p] - 1; i++) {
            for (int j = 0; j < priority_tasks_count[p] - i - 1; j++) {
                // If first task has no due date or second has earlier due date
                if (!priority_tasks[p][j]->due_date_set && priority_tasks[p][j+1]->due_date_set) {
                    // Swap (tasks without due dates go to the end)
                    task* temp = priority_tasks[p][j];
                    priority_tasks[p][j] = priority_tasks[p][j+1];
                    priority_tasks[p][j+1] = temp;
                }
                // Both have due date, compare them
                else if (priority_tasks[p][j]->due_date_set && priority_tasks[p][j+1]->due_date_set) {
                    if (compareDates(priority_tasks[p][j]->duedate, priority_tasks[p][j+1]->duedate) > 0) {
                        // Swap
                        task* temp = priority_tasks[p][j];
                        priority_tasks[p][j] = priority_tasks[p][j+1];
                        priority_tasks[p][j+1] = temp;
                    }
                }
            }
        }
    }
    
    // Write table header
    fprintf(file, "%-3s %-25s %-10s %-15s %-10s %-20s\n", "#", "Name", "Priority", "Due Date", "Status", "Tags");
    fprintf(file, "--------------------------------------------------------------------------------\n");
    
    // Export overdue tasks first
    int count = 1;
    for (int i = 0; i < overdue_tasks_count; i++) {
        task* t = overdue_tasks[i];
        
        char date_str[15] = "Not Set";
        if (t->due_date_set) {
            sprintf(date_str, "%02d/%02d/%04d", 
                   t->duedate.day, t->duedate.month, t->duedate.year);
        }
        
        char priority_str[10];
        switch(t->priority) {
            case 1: strcpy(priority_str, "High"); break;
            case 2: strcpy(priority_str, "Medium"); break;
            case 3: strcpy(priority_str, "Low"); break;
            default: strcpy(priority_str, "Unknown");
        }
        
        // Collect tags into a single string
        char tags_str[100] = "";
        for (int j = 0; j < t->tag_count; j++) {
            if (j > 0) strcat(tags_str, ", ");
            strcat(tags_str, t->tags[j]);
        }
        
        fprintf(file, "%-3d %-25s %-10s %-15s %-10s %-20s\n", 
               count++, t->name, priority_str, date_str, "OVERDUE", tags_str);
        
        total_exported++;
    }
    
    // Export pending tasks by priority
    for (int p = 0; p < 3; p++) {
        for (int i = 0; i < priority_tasks_count[p]; i++) {
            task* t = priority_tasks[p][i];
            
            char date_str[15] = "Not Set";
            if (t->due_date_set) {
                sprintf(date_str, "%02d/%02d/%04d", 
                       t->duedate.day, t->duedate.month, t->duedate.year);
            }
            
            char priority_str[10];
            switch(t->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            // Collect tags into a single string
            char tags_str[100] = "";
            for (int j = 0; j < t->tag_count; j++) {
                if (j > 0) strcat(tags_str, ", ");
                strcat(tags_str, t->tags[j]);
            }
            
            fprintf(file, "%-3d %-25s %-10s %-15s %-10s %-20s\n", 
                   count++, t->name, priority_str, date_str, "Pending", tags_str);
            
            total_exported++;
        }
    }
    
    // Separate section for completed tasks
    fprintf(file, "\n===== COMPLETED TASKS =====\n");
    fprintf(file, "%-3s %-25s %-10s %-15s %-20s\n", "#", "Name", "Priority", "Due Date", "Tags");
    fprintf(file, "--------------------------------------------------------------------------------\n");
    
    // Collect completed tasks from stack
    task* completed_tasks[200];
    int completed_tasks_count = 0;
    
    node = stack->top;
    while (node) {
        if (node->task_data) {
            completed_tasks[completed_tasks_count++] = node->task_data;
        }
        node = node->next;
    }
    
    // Export completed tasks
    count = 1;
    for (int i = 0; i < completed_tasks_count; i++) {
        task* t = completed_tasks[i];
        
        char date_str[15] = "Not Set";
        if (t->due_date_set) {
            sprintf(date_str, "%02d/%02d/%04d", 
                   t->duedate.day, t->duedate.month, t->duedate.year);
        }
        
        char priority_str[10];
        switch(t->priority) {
            case 1: strcpy(priority_str, "High"); break;
            case 2: strcpy(priority_str, "Medium"); break;
            case 3: strcpy(priority_str, "Low"); break;
            default: strcpy(priority_str, "Unknown");
        }
        
        // Collect tags into a single string
        char tags_str[100] = "";
        for (int j = 0; j < t->tag_count; j++) {
            if (j > 0) strcat(tags_str, ", ");
            strcat(tags_str, t->tags[j]);
        }
        
        fprintf(file, "%-3d %-25s %-10s %-15s %-20s\n", 
               count++, t->name, priority_str, date_str, tags_str);
        
        total_exported++;
    }
    
    // Add a more detailed summary at the end
    fprintf(file, "\n===== EXPORT SUMMARY =====\n");
    fprintf(file, "Total Tasks Exported: %d\n", total_exported);
    fprintf(file, "Pending Tasks: %d\n", pending_count);
    fprintf(file, "Overdue Tasks: %d\n", overdue_count);
    fprintf(file, "Completed Tasks: %d\n", completed_count);
    fprintf(file, "High Priority: %d\n", high_count);
    fprintf(file, "Medium Priority: %d\n", medium_count);
    fprintf(file, "Low Priority: %d\n", low_count);

    fclose(file);
    printf("Tasks exported to %s\n", filename);
    printf("Total %d tasks exported (%d pending, %d overdue, %d completed)\n",
           total_exported, pending_count, overdue_count, completed_count);
}


void importTasks(tasklist *list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for import");
        return;
    }

    char name[100];
    char desc[300];
    int priority, day, month, year;
    int imported_count = 0;
    char line[500]; // Buffer for reading lines

    // Skip header lines if they exist
    if (fgets(line, sizeof(line), file) != NULL) {
        // Check if it looks like a header line
        if (strstr(line, "===") != NULL || strstr(line, "TO-DO") != NULL) {
            // This is likely a header line, so read the next line
            fgets(line, sizeof(line), file);
        } else {
            // Not a header, rewind to start
            rewind(file);
        }
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        // Skip lines that are headers or separators
        if (strstr(line, "===") != NULL || strlen(line) < 5) {
            continue;
        }

        // Try to parse the line
        if (sscanf(line, " %99[^,],%299[^,],%d,%d/%d/%d", name, desc, &priority, &day, &month, &year) == 6) {
            // Trim whitespace from name and description
            char* end;
            
            // Trim trailing whitespace from name
            end = name + strlen(name) - 1;
            while (end > name && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';
            
            // Trim leading whitespace from name
            char* start = name;
            while (*start && isspace((unsigned char)*start)) start++;
            if (start != name) {
                memmove(name, start, strlen(start) + 1);
            }
            
            // Same for description
            end = desc + strlen(desc) - 1;
            while (end > desc && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';
            
            start = desc;
            while (*start && isspace((unsigned char)*start)) start++;
            if (start != desc) {
                memmove(desc, start, strlen(start) + 1);
            }

            // Check if a task with this name already exists
            if (isTaskNameDuplicate(list, name)) {
                printf("Warning: Task '%s' already exists. Skipping import.\n", name);
                continue;
            }

            // Validate date
            if (!isValidDate(day, month, year)) {
                printf("Warning: Invalid date (%d/%d/%d) for task '%s'. Setting no due date.\n", day, month, year, name);
                day = month = year = 0;
            }

            task* newtask = (task*)malloc(sizeof(task));
            if (!newtask) {
                printf("Memory allocation failed during import. Aborting rest.\n");
                break;
            }

            strcpy(newtask->name, name);
            strcpy(newtask->description, desc);
            
            // Validate priority (1-3)
            if (priority < 1 || priority > 3) {
                printf("Warning: Invalid priority %d for task '%s'. Setting to Medium (2).\n", priority, name);
                newtask->priority = 2;
            } else {
                newtask->priority = priority;
            }

            // Only set due date if it's valid
            if (day > 0 && month > 0 && year > 0) {
                newtask->duedate.day = day;
                newtask->duedate.month = month;
                newtask->duedate.year = year;
                newtask->due_date_set = 1;
            } else {
                newtask->due_date_set = 0;
            }

            newtask->completed = 0;        // Default to pending
            newtask->status = PENDING;

            newtask->next = list->head;
            list->head = newtask;
            imported_count++;
        } else {
            printf("Warning: Could not parse line: %s\n", line);
        }
    }

    if (ferror(file)) {
        perror("Error reading import file");
    }

    fclose(file);
    printf("%d tasks imported from %s\n", imported_count, filename);
}
