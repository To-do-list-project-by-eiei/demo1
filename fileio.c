#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

// Update the exportTasksTxt function in fileio.c to export all tasks organized by priority

void exportTasksTxt(task* head, completedstack* stack, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for export");
        return;
    }

    // Track how many tasks we export in each category
    int pending_count = 0, completed_count = 0;

    // Write a header to the file
    fprintf(file, "===== TO-DO LIST EXPORT =====\n");
    fprintf(file, "Date Exported: %s\n\n", __DATE__);

    // Export pending tasks by priority
    fprintf(file, "===== PENDING TASKS =====\n");
    
    // Process each priority level (1=High, 2=Medium, 3=Low)
    for (int priority = 1; priority <= 3; priority++) {
        switch (priority) {
            case 1: fprintf(file, "--- HIGH PRIORITY ---\n"); break;
            case 2: fprintf(file, "--- MEDIUM PRIORITY ---\n"); break;
            case 3: fprintf(file, "--- LOW PRIORITY ---\n"); break;
        }
        
        // Find tasks with this priority
        task* ptr = head;
        int found = 0;
        
        while (ptr) {
            if (ptr->priority == priority) {
                // Format: name,description,status,day/month/year
                fprintf(file, "Task: %s\n", ptr->name);
                fprintf(file, "Description: %s\n", ptr->description);
                fprintf(file, "Status: %s\n", 
                        ptr->status == PENDING ? "Pending" : 
                        ptr->status == OVERDUE ? "Overdue" : "Completed");
                        
                if (ptr->due_date_set) {
                    fprintf(file, "Due Date: %02d/%02d/%04d\n", 
                            ptr->duedate.day, ptr->duedate.month, ptr->duedate.year);
                } else {
                    fprintf(file, "Due Date: Not Set\n");
                }
                fprintf(file, "-------------------------\n");
                pending_count++;
                found = 1;
            }
            ptr = ptr->next;
        }
        
        if (!found) {
            fprintf(file, "No tasks with this priority.\n");
            fprintf(file, "-------------------------\n");
        }
    }

    // Export completed tasks (from stack)
    fprintf(file, "\n===== COMPLETED TASKS =====\n");
    
    stacknode* node = stack->top;
    if (!node) {
        fprintf(file, "No completed tasks.\n");
    } else {
        while (node) {
            if (node->task_data) {
                task* t = node->task_data;
                
                fprintf(file, "Task: %s\n", t->name);
                fprintf(file, "Description: %s\n", t->description);
                fprintf(file, "Priority: %d\n", t->priority);
                
                if (t->due_date_set) {
                    fprintf(file, "Due Date: %02d/%02d/%04d\n", 
                            t->duedate.day, t->duedate.month, t->duedate.year);
                } else {
                    fprintf(file, "Due Date: Not Set\n");
                }
                fprintf(file, "-------------------------\n");
                completed_count++;
            }
            node = node->next;
        }
    }

    // Add a summary at the end
    fprintf(file, "\n===== SUMMARY =====\n");
    fprintf(file, "Total Tasks: %d\n", pending_count + completed_count);
    fprintf(file, "Pending Tasks: %d\n", pending_count);
    fprintf(file, "Completed Tasks: %d\n", completed_count);

    fclose(file);
    printf("All tasks (%d pending, %d completed) exported to %s\n", 
           pending_count, completed_count, filename);
}

// Update the importTasks function in fileio.c to handle default filenames

void importTasks(tasklist *list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for import");
        return;
    }

    // Use generous buffer sizes, matching task struct fields
    char name[100];
    char desc[300];
    int priority, day, month, year;
    int imported_count = 0;

    // Format: name,description,priority,day/month/year
    // Use field widths in fscanf for safety against buffer overflows
    while (fscanf(file, " %99[^,],%299[^,],%d,%d/%d/%d\n", // Note leading space to skip whitespace
                   name, desc, &priority, &day, &month, &year) == 6) { // Expect 6 fields

        task* newtask = (task*)malloc(sizeof(task));
        if (!newtask) {
            printf("Memory allocation failed during import. Aborting rest.\n");
            break; // Stop importing if memory fails
        }

        strcpy(newtask->name, name);
        strcpy(newtask->description, desc);
        newtask->priority = priority;
        newtask->duedate.day = day;
        newtask->duedate.month = month;
        newtask->duedate.year = year;

        // Initialize other fields correctly for an imported (pending) task
        newtask->due_date_set = 1; // Assume date from file is valid
        newtask->completed = 0;
        newtask->status = PENDING; // Imported tasks are pending

        // Add to the beginning of the list
        newtask->next = list->head;
        list->head = newtask;
        imported_count++;
    }

    // Check for fscanf errors other than EOF
    if (ferror(file)) {
       perror("Error reading import file");
    }

    fclose(file);
    printf("%d tasks imported from %s\n", imported_count, filename);
}
