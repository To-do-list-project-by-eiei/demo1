#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"

void add(tasklist *list){

    task *newtask = (task*)malloc(sizeof(task));
    printf("Enter task name: ");
    scanf(" %[^\n]", newtask->name);
    printf("Enter description: ");
    scanf(" %[^\n]", newtask->description);

    do{
        printf("Enter priority(1 = High, 2 = Medium, 3 = Low): ");
        scanf("%d", &newtask->priority);
        if(newtask->priority < 1 || newtask->priority > 3){
            printf("Invalid priority. Please enter 1, 2, or 3.\n");
        }
    }while(newtask->priority < 1 || newtask->priority > 3);

    newtask->complete = 0;
    newtask->next = NULL;

    if(!list->head || newtask->priority < list->head->priority){
        newtask->next = list->head;
        list->head = newtask;
    }else{
        task *current = list->head;
        while(current->next && newtask->priority >= current->next->priority){
            current = current->next;
        }
        newtask->next = current->next;
        current->next = newtask;
    }
    printf("Task added successfully!\n");

}

void view(tasklist *list){

    task *current = list->head;
    if(!current){
        printf("No tasks to show.\n");
        return;
    }

    printf("\n--- Task List ---\n");
    while(current){
        if(!current->complete){
            printf("Name: %s\nDescription: %s\nPriority: %d\n\n",
            current->name, current->description, current->priority);
        }
        current = current->next;
    }
    
}

void edit(tasklist *list, const char *taskname){

    task *current = list->head;
    while(current && strcmp(current->name, taskname)!= 0){
        current = current->next;
    }

    if(!current){
        printf("Task not found.\n");
        return;
    }

    int choice;
    do{
        printf("Edit (1 = Name, 2 = Description, 3 = Priority): ");
        scanf("%d", &choice);
        if (choice < 1 || choice > 3) {
            printf("Invalid choice. Please enter 1, 2, or 3.\n");
        }
    }while(choice < 1 || choice > 3);

    switch(choice){
        case 1:
            printf("New name: ");
            scanf(" %[^\n]", current->name);
            break;
        case 2:
            printf("New description: ");
            scanf(" %[^\n]", current->description);
            break;
        case 3:
            do{
                printf("New priority (1 = High, 2 = Medium, 3 = Low): ");
                scanf("%d", &current->priority);
                if (current->priority < 1 || current->priority > 3) {
                    printf("Invalid priority. Please enter 1, 2, or 3.\n");
                }
            }while(current->priority < 1 || current->priority > 3);
            break;
    }
    printf("Task updated.\n");

}

void complete(tasklist *list, completedstack *stack, const char *taskname){

    task *current = list->head, *prev = NULL;
    while(current && strcmp(current->name, taskname)!= 0){
        prev = current;
        current = current->next;
    }
    if(!current){
        printf("Task not found.\n");
        return;
    }

    if(prev){
        prev->next = current->next;
    }else{
        list->head = current->next;
    }

    stacknode *node = (stacknode*)malloc(sizeof(stacknode));
    node->task = *current;
    node->next = stack->top;
    stack->top = node;

    free(current);
    printf("Task marked as completed.\n");

}

void undocompleted(tasklist *list, completedstack *stack){

    if(!stack->top){
        printf("No completed tasks to undo.\n");
        return;
    }

    stacknode *node = stack->top;
    stack->top = node->next;

    task *restored = (task*)malloc(sizeof(task));
    *restored = node->task;
    restored->next = list->head;
    list->head = restored;

    free(node);
    printf("Task restored: %s\n", restored->name);

}

void deletetask(tasklist *list, const char *taskname){

    task *current = list->head, *prev = NULL;
    while (current && strcmp(current->name, taskname)!= 0){
        prev = current;
        current = current->next;
    }
    if(!current){
        printf("Task not found.\n");
        return;
    }

    if(prev){
        prev->next = current->next;
    }else{
        list->head = current->next;
    }

    free(current);
    printf("Task deleted.\n");

}

void sort(tasklist *list, int criteria){

    if(!list->head || !list->head->next) return;

    task *sorted = NULL;

    while(list->head){
        task *current = list->head;
        list->head = list->head->next;

        if(!sorted || (criteria == 1 && current->priority < sorted->priority)){
            current->next = sorted;
            sorted = current;
        }else{
            task *temp = sorted;
            while(temp->next && temp->next->priority <= current->priority){
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
    }

    list->head = sorted;
    printf("Tasks sorted.\n");

}

void progress(tasklist *list, completedstack *stack){

    int total = 0, completed = 0;

    task *curr = list->head;
    while(curr){
        total++;
        curr = curr->next;
    }

    stacknode *node = stack->top;
    while(node){
        completed++;
        node = node->next;
    }

    printf("Total Tasks: %d | Completed: %d | Pending: %d | Progress: %.2f%%\n",
           total + completed, completed, total,
           (total + completed) ? ((float)completed / (total + completed)) * 100 : 0);

}