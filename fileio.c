#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

void exportTasksTxt(task* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for export"); // Use perror for system errors
        return;
    }

    task* ptr = head;
    int count = 0;
    while (ptr) {
        // Only export tasks that are PENDING (not completed)
        // Tasks in the 'head' list should generally be pending anyway with the new logic.
        if (!ptr->completed) {
             // Format: name,description,priority,day/month/year (Make sure no commas in name/desc!)
             fprintf(file, "%s,%s,%d,%d/%d/%d\n",
                     ptr->name, ptr->description, ptr->priority,
                     ptr->duedate.day, ptr->duedate.month, ptr->duedate.year);
             count++;
        }
        ptr = ptr->next;
    }
    fclose(file);
    printf("%d pending tasks exported to %s\n", count, filename);
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
