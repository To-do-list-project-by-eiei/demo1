#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"
#include "searchandstat.h"
#include "scheduler.h"
#include "fileio.h"



tasklist tasks = {NULL};
completedstack doneStack = {NULL};

void displayMenu(){
    printf("\n=== TO-DO LIST MENU ===\n");
    printf("1. Add Task\n");
    printf("2. View Tasks\n");
    printf("3. Edit Task\n");
    printf("4. Mark Task as Completed\n");
    printf("5. Undo Last Completed Task\n");
    printf("6. Delete Task\n");
    printf("7. Search Tasks\n");
    printf("8. View Statistics\n");
    printf("9. Set Due Date\n");
    printf("10. Simulate Day Change\n");
    printf("11. Export Tasks\n");
    printf("12. Clear Completed Tasks\n");
    printf("0. Exit\n");
    printf("Select an option: ");
}

int main(){
    int choice;
    while(1){
        displayMenu();
        scanf("%d", &choice);
        getchar();

        switch(choice){
            case 1: add(&tasks); break;
            case 2: view(&tasks); break;
            case 3: {
                char name[100];
                printf("Enter task name to edit: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                edit(&tasks, name);
                break;
            }
            case 4: {
                char name[100];
                printf("Enter task name to complete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                complete(&tasks, &doneStack, name);
                break;
            }
            case 5: undocompleted(&tasks, &doneStack); break;
            case 6: {
                char name[100];
                printf("Enter task name to delete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                deletetask(&tasks, name);
                break;
            }
            case 7: {
                char keyword[100];
                printf("Enter keyword to search: ");
                fgets(keyword, sizeof(keyword), stdin);
                keyword[strcspn(keyword, "\n")] = 0;
                search(tasks.head, keyword);
                break;
            }
            case 8: progress(&tasks, &doneStack); break;
            case 9: {
                char name[100];
                int day, month, year;
                printf("Enter task name to set due date: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Enter due date (DD MM YYYY): ");
                scanf("%d%d%d", &day, &month, &year);

                task* current = tasks.head;
                while(current){
                    if(strcmp(current->name, name) == 0){
                        setduedate(current, day, month, year);
                        printf("Due date set!\n");
                        break;
                    }
                    current = current->next;
                }
                break;
            }
            case 10: {
                date simdate;
                printf("Enter simulated date (DD MM YYYY): ");
                scanf("%d%d%d", &simdate.day, &simdate.month, &simdate.year);
                simulatedaychange(tasks.head, simdate);
                break;
            }
            case 11:
                exportTasks(tasks.head, "tasks_backup.csv");
                break;
            case 12:
                clearcompletedtask(&(doneStack.top));
                break;
            case 0:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Invalid option. Try again.\n");
        }
    }
}
