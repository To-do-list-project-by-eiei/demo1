#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"

void add(tasklist* list) {
    task* new_task = (task*)malloc(sizeof(task));
    if (!new_task) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter task name: ");
    fgets(new_task->name, sizeof(new_task->name), stdin);
    new_task->name[strcspn(new_task->name, "\n")] = 0;

    printf("Enter task description: ");
    fgets(new_task->description, sizeof(new_task->description), stdin);
    new_task->description[strcspn(new_task->description, "\n")] = 0;

    printf("Enter priority (1-High, 2-Medium, 3-Low): ");
    scanf("%d", &new_task->priority);
    getchar();

   

        printf("Enter due date (DD MM YYYY): ");
        scanf("%d %d %d", &new_task->duedate.day, &new_task->duedate.month, &new_task->duedate.year);
        getchar();
        new_task->due_date_set = 1;
   

    new_task->completed = 0;
    new_task->status = 0;
    new_task->next = list->head;
    list->head = new_task;

    printf("Task added successfully!\n");
}

void view(tasklist* list) {
    if (!list->head) {
        printf("\n=== Task List ===\n");
        printf("No tasks available.\n");
        return;
    }
    
    // Create arrays to group tasks by priority
    taskqueue priorityQueues[3]; // For priorities 1, 2, 3
    for (int i = 0; i < 3; i++) {
        initQueue(&priorityQueues[i]);
    }
    
    // First pass: Organize tasks by priority
    task* current = list->head;
    while (current) {
        int priority_index = current->priority - 1;
        if (priority_index >= 0 && priority_index < 3) {
            enqueue(&priorityQueues[priority_index], current);
        }
        current = current->next;
    }
    
    printf("\n=== Task List (Sorted by Priority and Due Date) ===\n");
    
    // Process each priority queue (1=High, 2=Medium, 3=Low)
    for (int p = 0; p < 3; p++) {
        // Create temporary arrays for sorting by date within each priority
        task* tempList = NULL;
        
        // Dequeue all tasks from this priority queue
        while (!isQueueEmpty(&priorityQueues[p])) {
            task* t = dequeue(&priorityQueues[p]);
            
            // Insert in date-sorted order
            if (!tempList || !t->due_date_set || 
                (tempList->due_date_set && compareDates(t->duedate, tempList->duedate) < 0)) {
                // Insert at beginning
                t->next = tempList;
                tempList = t;
            } else {
                // Insert in the middle or end
                task* prev = tempList;
                task* curr = tempList->next;
                
                while (curr && curr->due_date_set && 
                       (!t->due_date_set || compareDates(curr->duedate, t->duedate) <= 0)) {
                    prev = curr;
                    curr = curr->next;
                }
                
                t->next = curr;
                prev->next = t;
            }
        }
        
        // Display this priority group and its tasks (sorted by date)
        switch (p) {
            case 0: printf("\n--- HIGH PRIORITY TASKS ---\n"); break;
            case 1: printf("\n--- MEDIUM PRIORITY TASKS ---\n"); break;
            case 2: printf("\n--- LOW PRIORITY TASKS ---\n"); break;
        }
        
        // Print the sorted tasks
        current = tempList;
        while (current) {
            printf("Name: %s\n", current->name);
            printf("Description: %s\n", current->description);
            printf("Status: ");
            switch (current->status) {
                case PENDING: printf("Pending\n"); break;
                case COMPLETED: printf("Completed\n"); break;
                case OVERDUE: printf("Overdue\n"); break;
            }
            if (current->due_date_set) {
                printf("Due Date: %02d/%02d/%04d\n", 
                       current->duedate.day, current->duedate.month, current->duedate.year);
            } else {
                printf("Due Date: Not Set\n");
            }
            printf("-------------------------\n");
            
            task* next = current->next;
            current->next = list->head; // Restore the list structure
            current = next;
        }
        
        // Reset the temporary list without freeing the task nodes
        tempList = NULL;
    }
}


void edit(tasklist* list, const char* taskname) {
    task* current = list->head;
    while (current) {
        if (strcmp(current->name, taskname) == 0) {
            int choice;
            printf("Editing task: %s\n", current->name);
            printf("Choose what to edit:\n");
            printf("1. Name\n");
            printf("2. Description\n");
            printf("3. Priority\n");
            printf("4. Due Date\n");
            printf("Enter your choice (1-4): ");
            scanf("%d", &choice);
            getchar();  // To clear the newline from the input buffer

            switch (choice) {
                case 1:
                    printf("Enter new task name: ");
                    fgets(current->name, sizeof(current->name), stdin);
                    current->name[strcspn(current->name, "\n")] = 0;
                    break;
                case 2:
                    printf("Enter new description: ");
                    fgets(current->description, sizeof(current->description), stdin);
                    current->description[strcspn(current->description, "\n")] = 0;
                    break;
                case 3:
                    printf("Enter new priority (1-High, 2-Medium, 3-Low): ");
                    scanf("%d", &current->priority);
                    getchar();
                    break;
                case 4:
                    printf("Enter new due date (DD MM YYYY): ");
                    scanf("%d %d %d", &current->duedate.day, &current->duedate.month, &current->duedate.year);
                    getchar();
                    current->due_date_set = 1;
                    break;
                default:
                    printf("Invalid choice. Task not updated.\n");
                    return;
            }
            printf("Task updated successfully!\n");
            return;
        }
        current = current->next;
    }
    printf("Task not found.\n");
}


void complete(tasklist* list, completedstack* stack, const char* taskname) {
    task* current = list->head, *prev = NULL;
    while (current && strcmp(current->name, taskname) != 0) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        printf("Task not found.\n");
        return;
    }

    // Mark the task as completed BEFORE moving it
    current->status = COMPLETED;
    current->completed = 1;

    // Remove from list
    if (prev) prev->next = current->next;
    else list->head = current->next;

    // Allocate stack node
    stacknode* node = (stacknode*)malloc(sizeof(stacknode));
    if (!node) {
        printf("Memory allocation failed for stack node.\n");
        // CRITICAL: Should put the task back in the list or handle error better!
        // For simplicity now, just return, but the task is lost from view.
        // A robust solution would re-insert 'current' into 'list'.
        current->status = PENDING; // Revert status if stack fails
        current->completed = 0;
         if (prev) prev->next = current; else list->head = current; // Put back
        return;
    }

    // Push POINTER onto stack
    node->task_data = current; // Store the pointer to the original task node
    node->next = stack->top;
    stack->top = node;

    // DO NOT free(current); The stack now owns this task memory.
    printf("Task marked as completed and moved to stack!\n");
}

void undoCompleted(tasklist* list, completedstack* stack) {
    if (!stack->top) {
        printf("No completed tasks to undo.\n");
        return;
    }

    // Pop stack node
    stacknode* node = stack->top;
    stack->top = node->next;

    // Get the task POINTER back
    task* restored = node->task_data;

    // Update task status back to pending
    restored->status = PENDING;
    restored->completed = 0;

    // Add task back to the main list (at the head)
    restored->next = list->head;
    list->head = restored;

    free(node); // Free ONLY the stack node wrapper, not the task data
    printf("Last completed task restored to the list.\n");
}

void deleteTask(tasklist* list, const char* taskname) {
    task* current = list->head, *prev = NULL;
    while (current && strcmp(current->name, taskname) != 0) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        printf("Task not found.\n");
        return;
    }
    if (prev) prev->next = current->next;
    else list->head = current->next;
    free(current);
    printf("Task deleted.\n");
}

void sortTasks(tasklist* list) {
    if (!list->head || !list->head->next) return;
    task* sorted = NULL;
    while (list->head) {
        task* current = list->head;
        list->head = list->head->next;
        if (!sorted || current->priority < sorted->priority) {
            current->next = sorted;
            sorted = current;
        } else {
            task* temp = sorted;
            while (temp->next && temp->next->priority <= current->priority) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
    }
    list->head = sorted;
    printf("Tasks sorted by priority.\n");
}

void progress(tasklist* list, completedstack* stack) {
    int total = 0, done = 0;
    task* cur = list->head;
    while (cur) {
        total++;
        cur = cur->next;
    }
    stacknode* s = stack->top;
    while (s) {
        done++;
        s = s->next;
    }
    printf("Progress: %d tasks completed out of %d total (%.2f%%)\n",
           done, total + done,
           (total + done) ? (done * 100.0) / (total + done) : 0);
}

void freeTasks(tasklist* list) {

    task* current = list->head;
    
    while (current) {
    
    task* temp = current;
    
    current = current->next;
    
    free(temp);
    
    }
    
    }
    
// KEEP THIS VERSION:
void freeStack(completedstack* stack) {
    stacknode* current = stack->top;
    while (current) {
        stacknode* temp = current;
        current = current->next;
        // Check task_data just in case, then free it
        if (temp->task_data) {
             free(temp->task_data); // Free the actual task struct memory FIRST
        }
        free(temp);            // Free the stack node SECOND
    }
    stack->top = NULL; // Explicitly set top to NULL
}
void initQueue(taskqueue* q) {
    q->front = q->rear = NULL;
}

void enqueue(taskqueue* q, task* t) {
    queuenode* newNode = (queuenode*)malloc(sizeof(queuenode));
    if (!newNode) {
        printf("Memory allocation failed for queue node.\n");
        return;
    }
    
    newNode->task_data = t;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
}

task* dequeue(taskqueue* q) {
    if (q->front == NULL)
        return NULL;
    
    queuenode* temp = q->front;
    task* t = temp->task_data;
    
    q->front = q->front->next;
    
    if (q->front == NULL)
        q->rear = NULL;
    
    free(temp);
    return t;
}

int isQueueEmpty(taskqueue* q) {
    return q->front == NULL;
}

void freeQueue(taskqueue* q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
}

