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

    // Track how many tasks we export in each category
    int pending_count = 0, completed_count = 0, overdue_count = 0;
    int total_exported = 0;

    // Write a header to the file
    fprintf(file, "===== TO-DO LIST EXPORT =====\n");
    fprintf(file, "Date Exported: %s\n\n", __DATE__);

    // --- PENDING TASKS SECTION ---
    fprintf(file, "===== PENDING TASKS =====\n");

    // Create arrays to store tasks for sorting
    task* pending_tasks[200]; // Assuming max 200 pending tasks
    int pending_tasks_count = 0;

    // Collect all pending tasks
    task* current = head;
    while (current) {
        if (!current->completed && current->status == PENDING) {
            pending_tasks[pending_tasks_count++] = current;
        }
        current = current->next;
    }

    // Sort pending tasks by due date
    for (int i = 0; i < pending_tasks_count - 1; i++) {
        for (int j = 0; j < pending_tasks_count - i - 1; j++) {
            // If first task has no due date, or second task has due date and is earlier
            if (!pending_tasks[j]->due_date_set || 
                (pending_tasks[j+1]->due_date_set && 
                 pending_tasks[j]->due_date_set &&
                 compareDates(pending_tasks[j]->duedate, pending_tasks[j+1]->duedate) > 0)) {
                // Swap
                task* temp = pending_tasks[j];
                pending_tasks[j] = pending_tasks[j+1];
                pending_tasks[j+1] = temp;
            }
        }
    }

    // Export pending tasks sorted by due date
    if (pending_tasks_count > 0) {
        fprintf(file, "\n--- Sorted by Due Date (Soonest First) ---\n");
        
        for (int i = 0; i < pending_tasks_count; i++) {
            fprintf(file, "Task: %s\n", pending_tasks[i]->name);
            fprintf(file, "Description: %s\n", pending_tasks[i]->description);
            fprintf(file, "Priority: %d (%s)\n", pending_tasks[i]->priority, 
                  (pending_tasks[i]->priority == 1) ? "High" : 
                  (pending_tasks[i]->priority == 2) ? "Medium" : "Low");
            
            if (pending_tasks[i]->due_date_set) {
                fprintf(file, "Due Date: %02d/%02d/%04d\n",
                      pending_tasks[i]->duedate.day, 
                      pending_tasks[i]->duedate.month, 
                      pending_tasks[i]->duedate.year);
            } else {
                fprintf(file, "Due Date: Not Set\n");
            }
            fprintf(file, "-------------------------\n");
            pending_count++;
            total_exported++;
        }
    } else {
        fprintf(file, "No pending tasks.\n");
    }

    // --- OVERDUE TASKS SECTION ---
    fprintf(file, "\n===== OVERDUE TASKS =====\n");
    
    // Create arrays to store tasks for sorting
    task* overdue_tasks[200]; // Assuming max 200 overdue tasks
    int overdue_tasks_count = 0;

    // Collect all overdue tasks
    current = head;
    while (current) {
        if (!current->completed && current->status == OVERDUE) {
            overdue_tasks[overdue_tasks_count++] = current;
        }
        current = current->next;
    }

    // Sort overdue tasks by due date (most overdue first)
    for (int i = 0; i < overdue_tasks_count - 1; i++) {
        for (int j = 0; j < overdue_tasks_count - i - 1; j++) {
            if (overdue_tasks[j]->due_date_set && 
                overdue_tasks[j+1]->due_date_set && 
                compareDates(overdue_tasks[j]->duedate, overdue_tasks[j+1]->duedate) > 0) {
                // Swap
                task* temp = overdue_tasks[j];
                overdue_tasks[j] = overdue_tasks[j+1];
                overdue_tasks[j+1] = temp;
            }
        }
    }

    // Export overdue tasks sorted by due date
    if (overdue_tasks_count > 0) {
        fprintf(file, "\n--- Sorted by Due Date (Most Overdue First) ---\n");
        
        for (int i = 0; i < overdue_tasks_count; i++) {
            fprintf(file, "Task: %s\n", overdue_tasks[i]->name);
            fprintf(file, "Description: %s\n", overdue_tasks[i]->description);
            fprintf(file, "Priority: %d (%s)\n", overdue_tasks[i]->priority, 
                  (overdue_tasks[i]->priority == 1) ? "High" : 
                  (overdue_tasks[i]->priority == 2) ? "Medium" : "Low");
            fprintf(file, "Status: OVERDUE\n");
            
            if (overdue_tasks[i]->due_date_set) {
                fprintf(file, "Due Date: %02d/%02d/%04d (LATE)\n",
                      overdue_tasks[i]->duedate.day, 
                      overdue_tasks[i]->duedate.month, 
                      overdue_tasks[i]->duedate.year);
            } else {
                fprintf(file, "Due Date: Not Set\n");
            }
            fprintf(file, "-------------------------\n");
            overdue_count++;
            total_exported++;
        }
    } else {
        fprintf(file, "No overdue tasks.\n");
    }

    // --- COMPLETED TASKS SECTION ---
    fprintf(file, "\n===== COMPLETED TASKS =====\n");

    // Create array to store completed tasks for sorting
    task* completed_tasks[200]; // Assuming max 200 completed tasks
    int completed_tasks_count = 0;

    // Collect all completed tasks from the stack
    stacknode* node = stack->top;
    while (node) {
        if (node->task_data) {
            completed_tasks[completed_tasks_count++] = node->task_data;
        }
        node = node->next;
    }

    // Sort completed tasks by due date
    for (int i = 0; i < completed_tasks_count - 1; i++) {
        for (int j = 0; j < completed_tasks_count - i - 1; j++) {
            // Skip if either task has no due date
            if (!completed_tasks[j]->due_date_set || !completed_tasks[j+1]->due_date_set) {
                continue;
            }
            
            if (compareDates(completed_tasks[j]->duedate, completed_tasks[j+1]->duedate) > 0) {
                // Swap
                task* temp = completed_tasks[j];
                completed_tasks[j] = completed_tasks[j+1];
                completed_tasks[j+1] = temp;
            }
        }
    }

    // Export completed tasks sorted by due date
    if (completed_tasks_count > 0) {
        fprintf(file, "\n--- Most Recent Completions First ---\n");
        
        // The stack is already ordered by completion time (most recent at top)
        // So we can just export in the order of the stack
        for (int i = 0; i < completed_tasks_count; i++) {
            fprintf(file, "Task: %s\n", completed_tasks[i]->name);
            fprintf(file, "Description: %s\n", completed_tasks[i]->description);
            fprintf(file, "Priority: %d (%s)\n", completed_tasks[i]->priority, 
                  (completed_tasks[i]->priority == 1) ? "High" : 
                  (completed_tasks[i]->priority == 2) ? "Medium" : "Low");
            fprintf(file, "Status: Completed\n");
            
            if (completed_tasks[i]->due_date_set) {
                fprintf(file, "Due Date: %02d/%02d/%04d\n",
                      completed_tasks[i]->duedate.day, 
                      completed_tasks[i]->duedate.month, 
                      completed_tasks[i]->duedate.year);
            } else {
                fprintf(file, "Due Date: Not Set\n");
            }
            fprintf(file, "-------------------------\n");
            completed_count++;
            total_exported++;
        }
    } else {
        fprintf(file, "No completed tasks.\n");
    }

    // Add a summary at the end
    fprintf(file, "\n===== SUMMARY =====\n");
    fprintf(file, "Total Tasks Exported: %d\n", total_exported);
    fprintf(file, "Pending Tasks: %d\n", pending_count);
    fprintf(file, "Overdue Tasks: %d\n", overdue_count);
    fprintf(file, "Completed Tasks: %d\n", completed_count);

    fclose(file);
    printf("Tasks exported to %s\n", filename);
    printf("Summary: %d pending, %d overdue, %d completed (Total Exported: %d)\n",
           pending_count, overdue_count, completed_count, total_exported);
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
