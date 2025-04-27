#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"
#include "searchandstat.h"
#include "scheduler.h"
#include "fileio.h"

tasklist tasks = {NULL};
completedstack doneStack = {NULL};

void pause() {
    printf("\nPress Enter to continue...");
    getchar();
}

void displayMenu() {
    printf("\n=== TO-DO LIST MENU ===\n");
    printf("1. Add Task\n");
    printf("2. View Tasks\n");
    printf("3. Edit Task\n");
    printf("4. Mark Task as Completed\n");
    printf("5. Undo Last Completed Task\n");
    printf("6. Delete Task\n");
    printf("7. Search Tasks\n");
    printf("8. View Statistics (Progress)\n");
    printf("9. Clear All Completed Tasks\n");
    printf("10. Import Tasks\n");
    printf("11. Export Tasks\n");
    printf("12. Daily Completed Tasks\n");
    printf("13. Simulate Day Change\n");
    printf("0. Exit\n");
    printf("Select an option: ");
}

int main() {
    int choice;
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        getchar(); // flush newline

        switch (choice) {
            case 1: 
                add(&tasks);
                pause();
                break;
            case 2: 
                view(&tasks);
                pause();
                break;
            case 3: {
                char name[100];
                printf("Enter task name to edit: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                edit(&tasks, name);
                pause();
                break;
            }
            case 4: {
                char name[100];
                printf("Enter task name to complete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                complete(&tasks, &doneStack, name);
                pause();
                break;
            }
            case 5:
                undoCompleted(&tasks, &doneStack);
                pause();
                break;
            case 6: {
                char name[100];
                printf("Enter task name to delete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                deleteTask(&tasks, name);
                pause();
                break;
            }
            case 7: {
                char keyword[100];
                printf("Enter keyword to search: ");
                fgets(keyword, sizeof(keyword), stdin);
                keyword[strcspn(keyword, "\n")] = 0;
                searchTasks(tasks.head, &doneStack, keyword);
                pause();
                break;
            }
            case 8:
                showStats(tasks.head, &doneStack);
                pause();
                break;
                case 9:
                clearcompletedtask(&(doneStack.top)); // <-- Your function from scheduler.h
                pause();
                break;
            
                case 10: {
                    char filename[100];
                    printf("Enter filename to import (default: tasks_import.txt): ");
                    fgets(filename, sizeof(filename), stdin);
                    filename[strcspn(filename, "\n")] = 0;
                    
                    if (strlen(filename) == 0) {
                        strcpy(filename, "tasks_import.txt");
                    }
                    
                    importTasks(&tasks, filename);
                    pause();
                    break;
                }

            case 11: {
                    char filename[100];
                    printf("Enter filename for export (default: tasks_export.txt): ");
                    fgets(filename, sizeof(filename), stdin);
                    filename[strcspn(filename, "\n")] = 0;
                    
                    if (strlen(filename) == 0) {
                        strcpy(filename, "tasks_export.txt");
                    }
                    
                    exportTasksTxt(tasks.head, &doneStack, filename);
                    pause();
                    break;
                }

            case 12:
                doneToday(&tasks, &doneStack);
                pause();
                break;
            case 13:
                simulateDayChange(tasks.head, getToday());
                adjustPriority(tasks.head, getToday());
                pause();
                break;
            
            case 0:
                printf("Exiting...\n");
                freeTasks(&tasks);
                freeStack(&doneStack);
                exit(0);
            default:
                printf("Invalid option. Try again.\n");
                pause();
        }
    }        
}
