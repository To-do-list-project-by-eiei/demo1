#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"

int main() {
    tasklist mylist = {NULL};
    completedstack mystack = {NULL};

    int choice;
    char name[100];

    do {
        printf("\n--- TO-DO LIST MENU ---\n");
        printf("1. Add Task\n");
        printf("2. View Tasks\n");
        printf("3. Edit Task\n");
        printf("4. Complete Task\n");
        printf("5. Undo Completed Task\n");
        printf("6. Delete Task\n");
        printf("7. Sort Tasks by Priority\n");
        printf("8. View Progress\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear leftover newline

        switch (choice) {
            case 1:
                add(&mylist);
                break;
            case 2:
                view(&mylist);
                break;
            case 3:
                printf("Enter task name to edit: ");
                scanf(" %[^\n]", name);
                edit(&mylist, name);
                break;
            case 4:
                printf("Enter task name to complete: ");
                scanf(" %[^\n]", name);
                complete(&mylist, &mystack, name);
                break;
            case 5:
                undocompleted(&mylist, &mystack);
                break;
            case 6:
                printf("Enter task name to delete: ");
                scanf(" %[^\n]", name);
                deletetask(&mylist, name);
                break;
            case 7:
                sort(&mylist, 1); // 1 = priority sorting
                break;
            case 8:
                progress(&mylist, &mystack);
                break;
            case 9:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 9);

    return 0;
}