#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

// Export tasks to a text file
void exportTasks(task* head, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open file for writing.\n");
        return;
    }

    task *current = head;
    while (current) {
        fprintf(fp, "%s|%s|%d|%d|%d-%d-%d\n",
            current->name,
            current->description,
            current->priority,
            current->complete,
            current->duedate.day,
            current->duedate.month,
            current->duedate.year);
        current = current->next;
    }

    fclose(fp);
    printf("Tasks exported to %s successfully.\n", filename);
}

// Import tasks from a text file
void importTasks(tasklist *list, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file for reading.\n");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        task *newtask = (task *)malloc(sizeof(task));
        if (newtask == NULL) {
            printf("Memory allocation failed.\n");
            fclose(fp);
            return;
        }

        sscanf(line, " %[^|]|%[^|]|%d|%d|%d-%d-%d",
            newtask->name,
            newtask->description,
            &newtask->priority,
            &newtask->complete,
            &newtask->duedate.day,
            &newtask->duedate.month,
            &newtask->duedate.year);

        newtask->next = list->head;
        list->head = newtask;
    }

    fclose(fp);
    printf("Tasks imported from %s successfully.\n", filename);
}
