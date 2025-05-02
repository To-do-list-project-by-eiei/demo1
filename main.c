#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "task_management.h"
#include "searchandstat.h"
#include "fileio.h"

tasklist tasks = {NULL};
completedstack doneStack = {NULL};

date currentDate;

void pause() {
    printf("\nPress Enter to continue...");
    getchar();
}

int checkForLoops(tasklist* list) {
    if (!list->head) return 0;  // Empty list has no loops
    
    // Floyd's Cycle-Finding Algorithm (Tortoise and Hare)
    task* slow = list->head;
    task* fast = list->head->next;
    
    while (fast && fast->next) {
        if (slow == fast) {
            // Loop detected
            return 1;
        }
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return 0;  // No loops found
}

void debugTaskList() {
    printf("\n=== Debugging Task List ===\n");
    
    // Check if tasks list has loops
    if (checkForLoops(&tasks)) {
        printf("ERROR: Loop detected in tasks list!\n");
    } else {
        printf("Task list integrity: OK\n");
    }
    
    // Count tasks and check for NULL/invalid data
    int count = 0;
    task* current = tasks.head;
    while (current && count < 1000) {  // Safety limit
        count++;
        
        // Check for invalid data
        if (current->priority < 1 || current->priority > 3) {
            printf("WARNING: Task '%s' has invalid priority: %d\n", 
                   current->name, current->priority);
        }
        
        if (current->due_date_set && 
            (current->duedate.day < 1 || current->duedate.day > 31 ||
             current->duedate.month < 1 || current->duedate.month > 12 ||
             current->duedate.year < 2000 || current->duedate.year > 2100)) {
            printf("WARNING: Task '%s' has suspicious date: %d/%d/%d\n",
                   current->name, current->duedate.day, 
                   current->duedate.month, current->duedate.year);
        }
        
        current = current->next;
    }
    
    if (count == 1000) {
        printf("WARNING: Possible infinite loop in task list (1000+ items)\n");
    } else {
        printf("Task count: %d\n", count);
    }
    
    // Check completed stack
    count = 0;
    stacknode* node = doneStack.top;
    while (node && count < 1000) {  // Safety limit
        count++;
        
        // Check for NULL task data
        if (!node->task_data) {
            printf("ERROR: Stack node #%d has NULL task data!\n", count);
        }
        
        node = node->next;
    }
    
    if (count == 1000) {
        printf("WARNING: Possible infinite loop in completed stack\n");
    } else {
        printf("Completed task count: %d\n", count);
    }
    
    printf("=== End Debugging ===\n\n");
}

void displayMenu() {
    printf("\n=== TO-DO LIST MENU ===\n");
    printf("1. Add Task\n");
    printf("2. View Tasks (Standard/Simplified/By Tag)\n");
    printf("3. Edit Task\n");
    printf("4. Mark Task as Completed\n");
    printf("5. Undo Last Completed Task\n");
    printf("6. Delete Task\n");
    printf("7. Search Tasks\n");
    printf("8. View Statistics (All/Week/Month)\n");
    printf("9. Clear All Completed Tasks\n");
    printf("10. Import Tasks\n");
    printf("11. Export Tasks\n");
    printf("12. Daily Completed Tasks\n");
    printf("13. Simulate Day Change\n");
    printf("14. Time Period Summary (Week/Month)\n");
    printf("15. Add Tag to Task\n");
    printf("0. Exit\n");
    printf("Select an option: ");
}


int main() {
    int choice;

    currentDate = getToday();

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
                view_combined(&tasks, currentDate);  // Combined view function
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
                searchTasks(tasks.head, &doneStack, keyword);
                pause();
                break;
            }
            case 8:
                show_combined_stats(tasks.head, &doneStack, currentDate);  // Combined stats function
                pause();
                break;
            case 9:
                clearcompletedtask(&(doneStack.top));
                pause();
                break;
            case 10: {
                char filename[100];
                printf("Enter filename to import (default: tasks_import.txt, sample_tasks.txt): ");
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
                simulateDayChange(tasks.head, &currentDate);
                pause();
                break;
            case 14:
                view_time_summary(&tasks, currentDate);  // Combined time summary function
                pause();
                break;
            case 15: {
                char name[100];
                printf("Enter task name to add tag: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                add_tag_to_task(&tasks, name);
                pause();
                break;
            }
            
            case 99:  // Hidden debug option
                debugTaskList();
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


