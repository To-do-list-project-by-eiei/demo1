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
    int pending_count = 0, completed_count = 0, overdue_count = 0;
    int total_exported = 0;

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

        // Find tasks with this priority that are NOT completed
        task* ptr = head;
        int found = 0;

        while (ptr) {
            if (!ptr->completed && ptr->priority == priority) { // Check if NOT completed and has the current priority
                fprintf(file, "Task: %s\n", ptr->name);
                fprintf(file, "Description: %s\n", ptr->description);
                fprintf(file, "Priority: %d\n", ptr->priority);

                // Process status (should be PENDING or OVERDUE for non-completed tasks)
                if (ptr->status == PENDING) {
                    fprintf(file, "Status: Pending\n");
                    pending_count++;
                } else if (ptr->status == OVERDUE) {
                    fprintf(file, "Status: OVERDUE\n");
                    overdue_count++;
                } else {
                    fprintf(file, "Status: Unknown (Should not happen for pending)\n");
                }

                if (ptr->due_date_set) {
                    fprintf(file, "Due Date: %02d/%02d/%04d\n",
                            ptr->duedate.day, ptr->duedate.month, ptr->duedate.year);
                } else {
                    fprintf(file, "Due Date: Not Set\n");
                }
                fprintf(file, "-------------------------\n");
                found = 1;
                total_exported++;
            }
            ptr = ptr->next;
        }

        if (!found) {
            fprintf(file, "No pending/overdue tasks with this priority.\n");
            fprintf(file, "-------------------------\n");
        }
    }

    // Export completed tasks (from stack)
    fprintf(file, "\n===== COMPLETED TASKS =====\n");

    stacknode* node = stack->top;
    int stack_count = 0;

    if (!node) {
        fprintf(file, "No completed tasks.\n");
    } else {
        while (node) {
            if (node->task_data) {
                task* t = node->task_data;

                fprintf(file, "Task: %s\n", t->name);
                fprintf(file, "Description: %s\n", t->description);
                fprintf(file, "Priority: %d\n", t->priority);
                fprintf(file, "Status: Completed\n");

                if (t->due_date_set) {
                    fprintf(file, "Due Date: %02d/%02d/%04d\n",
                            t->duedate.day, t->duedate.month, t->duedate.year);
                } else {
                    fprintf(file, "Due Date: Not Set\n");
                }
                fprintf(file, "-------------------------\n");
                completed_count++;
                total_exported++;
                stack_count++;
            }
            node = node->next;
        }
    }

    // First, count total pending/overdue tasks
    int total_pending_overdue = 0;
    task* current_pending = head;
    while (current_pending) {
        total_pending_overdue++;
        current_pending = current_pending->next;
    }

    // Then, the stack count already gives the number of completed tasks:
    int total_completed = stack_count;

    // The true total of tasks ever managed is the sum of these two:
    int total_tasks_ever = total_pending_overdue + total_completed;

    // Add a summary at the end
    fprintf(file, "\n===== SUMMARY =====\n");
    fprintf(file, "Total Tasks in Program (Active + Completed): %d\n", total_tasks_ever);
    fprintf(file, "Total Tasks Exported: %d\n", total_exported);
    fprintf(file, "Pending Tasks: %d\n", pending_count);
    fprintf(file, "Overdue Tasks: %d\n", overdue_count);
    fprintf(file, "Completed Tasks: %d\n", completed_count);

    // ... (rest of your exportTasksTxt function) ...

    printf("(Program contains %d tasks in list + %d completed)\n",
           total_pending_overdue, total_completed);
}


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
