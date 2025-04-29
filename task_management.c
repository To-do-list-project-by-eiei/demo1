#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   // Include for isspace()
#include <stdbool.h>
#include "task_management.h"

void add(tasklist* list) {
    task* new_task = (task*)malloc(sizeof(task));
    if (!new_task) {
        printf("Memory allocation failed.\n");
        return;
    }

    char task_name[100];
    bool is_valid_name = false; // Flag to control the loop

    do {
        printf("Enter task name: ");
        if (fgets(task_name, sizeof(task_name), stdin) == NULL) {
             // Handle potential input error (e.g., EOF)
             printf("Error reading input.\n");
             free(new_task); // Clean up allocated memory
             return;
        }
        task_name[strcspn(task_name, "\n")] = 0; // Remove trailing newline

        // --- Input Validation Start ---
        is_valid_name = true; // Assume valid initially for this iteration
        bool contains_only_whitespace = true;

        // 1. Check if the string is empty
        if (task_name[0] == '\0') {
            printf("Error: Task name cannot be empty. Please enter a valid name.\n");
            is_valid_name = false;
        } else {
            // 2. Check if the string contains only whitespace
            for (int i = 0; task_name[i] != '\0'; i++) {
                // isspace checks for space, tab, newline, etc.
                // Cast to unsigned char for safety with ctype functions
                if (!isspace((unsigned char)task_name[i])) {
                    contains_only_whitespace = false;
                    break; // Found a non-whitespace character, no need to check further
                }
            }

            if (contains_only_whitespace) {
                printf("Error: Task name cannot consist only of whitespace. Please enter a valid name.\n");
                is_valid_name = false;
            }
        }
        // --- Input Validation End ---

        // 3. Check for duplicates only if the name format is valid
        if (is_valid_name) {
            if (isTaskNameDuplicate(list, task_name)) {
                printf("Error: A task with this name already exists. Please choose a different name.\n");
                is_valid_name = false; // Mark as invalid to loop again
            }
        }
        // Loop continues if the name was invalid (empty, whitespace, or duplicate)
    } while (!is_valid_name);

    // Copy validated name to the task struct
    strcpy(new_task->name, task_name);

    printf("Enter task description: ");
    // Consider adding similar validation for description if needed
    fgets(new_task->description, sizeof(new_task->description), stdin);
    new_task->description[strcspn(new_task->description, "\n")] = 0;

    printf("Enter priority (1-High, 2-Medium, 3-Low): ");
    // Be careful with mixing fgets and scanf. Using fgets for all input is safer.
    // Example of safer input for integer:
    char buffer[20];
    int priority_input = 0;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &priority_input) == 1) {
             new_task->priority = priority_input;
        } else {
            printf("Invalid priority input. Setting to Medium (2).\n");
            new_task->priority = 2; // Default on parse error
        }
    } else {
        printf("Error reading priority input. Setting to Medium (2).\n");
        new_task->priority = 2; // Default on read error
    }
    // No need for getchar() here if using fgets exclusively

    // Validate priority range
    if (new_task->priority < 1 || new_task->priority > 3) {
        printf("Invalid priority value. Setting to Medium (2).\n");
        new_task->priority = 2;
    }

    printf("Enter due date (DD MM YYYY): ");
     // Similarly, safer to read the whole line with fgets and parse
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d %d %d", &new_task->duedate.day, &new_task->duedate.month, &new_task->duedate.year) == 3) {
            new_task->due_date_set = 1;
             // Basic date validation
            if (new_task->duedate.day < 1 || new_task->duedate.day > 31 ||
                new_task->duedate.month < 1 || new_task->duedate.month > 12 ||
                new_task->duedate.year < 2000) { // Consider a more robust date check
                printf("Warning: Date might be invalid (Day: %d, Month: %d, Year: %d). Using it anyway.\n",
                       new_task->duedate.day, new_task->duedate.month, new_task->duedate.year);
            }
        } else {
             printf("Invalid date format. Due date not set.\n");
             new_task->due_date_set = 0;
        }
    } else {
        printf("Error reading date input. Due date not set.\n");
        new_task->due_date_set = 0;
    }
    // No need for getchar() here if using fgets exclusively

    new_task->completed = 0;
    // Assuming 'PENDING' is defined elsewhere (e.g., an enum)
    // new_task->status = PENDING;
    new_task->next = list->head;
    list->head = new_task;

    printf("Task added successfully!\n");
}


// Function to check if a task name already exists
int isTaskNameDuplicate(tasklist* list, const char* name) {
    task* current = list->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return 1; // Name exists
        }
        current = current->next;
    }
    return 0; // Name doesn't exist
}

void view(tasklist* list, date today) {
    task* current = list->head;
    printf("\n=== Task List ===\n");
    
    if (!current) {
        printf("No tasks available.\n");
        return;
    }
    
    // Create arrays to group tasks by priority
    taskqueue priorityQueues[3]; // For priorities 1, 2, 3
    for (int i = 0; i < 3; i++) {
        initQueue(&priorityQueues[i]);
    }
    
    // First, update statuses based on current date
    updateTaskStatuses(current, today);
    
    // Organize tasks by priority
    while (current) {
        int priority_index = current->priority - 1;
        if (priority_index >= 0 && priority_index < 3) {
            enqueue(&priorityQueues[priority_index], current);
        }
        current = current->next;
    }
    
    // Display tasks by priority and due date
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
        // Print the sorted tasks
current = tempList;
int taskCount = 0;
task* next_temp;
while (current) {
    printf("Name: %s", current->name);
    
    // Show urgent tag for tasks due soon
    if (!current->completed && current->due_date_set && isDateSoon(today, current->duedate, 2)) {
        printf(" [!]URGENT");
    }
    printf("\n");
    
    printf("Description: %s\n", current->description);
    printf("Status: ");
    switch (current->status) {
        case PENDING: printf("Pending\n"); break;
        case COMPLETED: printf("Completed\n"); break;
        case OVERDUE: printf("OVERDUE\n"); break;
    }
    if (current->due_date_set) {
        printf("Due Date: %02d/%02d/%04d\n", 
               current->duedate.day, current->duedate.month, current->duedate.year);
    } else {
        printf("Due Date: Not Set\n");
    }
    printf("-------------------------\n");
    
    next_temp = current->next;
    current = next_temp;
    taskCount++;
}
        if (taskCount == 0) {
            printf("No tasks with this priority.\n");
            printf("-------------------------\n");
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
            char buffer[20]; // Buffer for reading numeric/choice input safely

            printf("Editing task: %s\n", current->name);
            printf("Choose what to edit:\n");
            printf("1. Name\n");
            printf("2. Description\n");
            printf("3. Priority\n");
            printf("4. Due Date\n");
            printf("Enter your choice (1-4): ");

            // Safer choice input
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
                printf("Invalid choice input. Aborting edit.\n");
                return;
            }
            // No getchar() needed after fgets

            switch (choice) {
                case 1: {
                    char new_name[100];
                    bool is_duplicate; // Renamed flag for clarity
                    bool format_valid_and_not_same; // Flag to control loop exit and final copy

                    do {
                        printf("Enter new task name: ");
                        if (fgets(new_name, sizeof(new_name), stdin) == NULL) {
                            printf("Error reading input. Aborting name change.\n");
                            return; // Or handle error differently
                        }
                        new_name[strcspn(new_name, "\n")] = 0; // Remove newline

                        // --- Input Validation Start ---
                        bool is_valid_format = true;
                        bool contains_only_whitespace = true;

                        // 1. Check if empty
                        if (new_name[0] == '\0') {
                            printf("Error: Task name cannot be empty. Please enter a valid name.\n");
                            is_valid_format = false;
                        } else {
                            // 2. Check if only whitespace
                            for (int i = 0; new_name[i] != '\0'; i++) {
                                if (!isspace((unsigned char)new_name[i])) {
                                    contains_only_whitespace = false;
                                    break;
                                }
                            }
                            if (contains_only_whitespace) {
                                printf("Error: Task name cannot consist only of whitespace. Please enter a valid name.\n");
                                is_valid_format = false;
                            }
                        }
                        // --- Input Validation End ---

                        if (!is_valid_format) {
                             is_duplicate = true; // Force loop repeat if format is bad
                             format_valid_and_not_same = false;
                             continue; // Skip further checks for this iteration
                        }

                        // 3. Check if the new name is the same as the current one
                        if (strcmp(new_name, current->name) == 0) {
                            printf("The new name is the same as the current name. No change needed.\n");
                             // If the name is the same, we don't need to check for duplicates
                             // or copy it. We can exit the loop and skip the copy.
                            is_duplicate = false; // It's not a duplicate of *another* task
                            format_valid_and_not_same = false; // Signal not to copy below
                            break; // Exit the do-while loop
                        }

                        // 4. Check if it duplicates *another* existing task name
                        is_duplicate = isTaskNameDuplicate(list, new_name);
                        if (is_duplicate) {
                            printf("Error: A task with this name already exists. Please choose a different name.\n");
                            format_valid_and_not_same = false; // Ensure we loop again
                        } else {
                            // Format is valid, it's different from the old name, and not a duplicate
                            format_valid_and_not_same = true; // Name is acceptable
                        }

                    } while (is_duplicate || !format_valid_and_not_same); // Loop if duplicate OR format was invalid/same

                    // Only copy if the loop finished with a valid, different, non-duplicate name
                    if (format_valid_and_not_same) {
                         strcpy(current->name, new_name);
                         printf("Task name updated.\n"); // Give specific feedback
                    }
                    // If format_valid_and_not_same is false, it's because the name entered was the same as the old one.
                    // No error, but no update needed. Message already printed inside loop.

                    break; // Break from switch case 1
                } // end case 1
                case 2:
                    printf("Enter new description: ");
                    // Assuming description can be empty or whitespace, no validation added here
                    // but you could add similar checks if needed.
                    fgets(current->description, sizeof(current->description), stdin);
                    current->description[strcspn(current->description, "\n")] = 0;
                    printf("Task description updated.\n");
                    break;
                case 3: {
                    int priority_input;
                    printf("Enter new priority (1-High, 2-Medium, 3-Low): ");
                    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                        if (sscanf(buffer, "%d", &priority_input) == 1) {
                            // Validate priority range
                            if (priority_input >= 1 && priority_input <= 3) {
                                current->priority = priority_input;
                                printf("Task priority updated.\n");
                            } else {
                                printf("Invalid priority value (%d). Priority not changed.\n", priority_input);
                            }
                        } else {
                            printf("Invalid priority input format. Priority not changed.\n");
                        }
                    } else {
                        printf("Error reading priority input. Priority not changed.\n");
                    }
                    // No getchar() needed
                    break;
                } // end case 3
                case 4: {
                    // Using fgets for y/n is tricky. Let's stick to scanf + getchar for single char.
                    // Alternatively read line with fgets and check buffer[0]
                    char has_due_date;
                    printf("Update the due date? (y/n): "); // Changed wording slightly
                    // Read the character using fgets to be safer
                    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                         has_due_date = buffer[0]; // Take the first character

                         if (has_due_date == 'y' || has_due_date == 'Y') {
                            printf("Enter new due date (DD MM YYYY): ");
                            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                                int d, m, y;
                                if (sscanf(buffer, "%d %d %d", &d, &m, &y) == 3) {
                                     // Basic date validation
                                    if (d < 1 || d > 31 || m < 1 || m > 12 || y < 2000) { // Basic check
                                        printf("Warning: Date might be invalid (Day: %d, Month: %d, Year: %d). Using it anyway.\n", d, m, y);
                                    }
                                    current->duedate.day = d;
                                    current->duedate.month = m;
                                    current->duedate.year = y;
                                    current->due_date_set = 1;
                                    printf("Task due date updated.\n");
                                } else {
                                    printf("Invalid date format. Due date not changed.\n");
                                    // Keep existing due date status (current->due_date_set remains unchanged)
                                }
                            } else {
                                printf("Error reading date input. Due date not changed.\n");
                            }
                         } else if (has_due_date == 'n' || has_due_date == 'N') {
                            // Option to explicitly clear the due date?
                             printf("Do you want to clear the existing due date? (y/n): ");
                             if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                                if (buffer[0] == 'y' || buffer[0] == 'Y') {
                                     current->due_date_set = 0;
                                     printf("Due date cleared.\n");
                                } else {
                                     printf("Due date not changed.\n");
                                }
                             } else {
                                printf("Error reading input. Due date not changed.\n");
                             }
                         } else {
                             printf("Invalid choice ('%c'). Due date not changed.\n", has_due_date);
                         }
                    } else {
                        printf("Error reading input. Due date not changed.\n");
                    }
                    break;
                } // end case 4
                default:
                    printf("Invalid choice. Task not updated.\n");
                    return; // Return here as no valid update was chosen
            }
            // Only print "Task updated successfully!" if an actual change was made.
            // The individual cases now print specific success messages.
            // Maybe remove this general message or rephrase.
            // printf("Task update process finished.\n");
            return; // Exit function after successful edit or handling a specific case
        }
        current = current->next;
    }
    printf("Task '%s' not found.\n", taskname); // Clarify which task wasn't found
}


void complete(tasklist* list, completedstack* stack, const char* taskname) {
    if (!list || !stack || !taskname) {
        printf("Error: Invalid parameters for complete function.\n");
        return;
    }
    
    task* current = list->head;
    task* prev = NULL;
    
    // Find the task to complete
    while (current && strcmp(current->name, taskname) != 0) {
        prev = current;
        current = current->next;
    }
    
    if (!current) {
        printf("Task not found: %s\n", taskname);
        return;
    }
    
    // Debug info
    printf("Found task: %s (Priority: %d)\n", current->name, current->priority);
    
    // Allocate stack node first to check for memory issues
    stacknode* node = (stacknode*)malloc(sizeof(stacknode));
    if (!node) {
        printf("Memory allocation failed for stack node. Task remains in list.\n");
        return;
    }
    
    // Mark the task as completed
    current->status = COMPLETED;
    current->completed = 1;
    
    // Remove from list first
    if (prev) {
        prev->next = current->next;
    } else {
        list->head = current->next;
    }
    
    // Clear the next pointer to avoid circular references
    current->next = NULL;
    
    // Push onto stack
    node->task_data = current;
    node->next = stack->top;
    stack->top = node;
    
    printf("Task '%s' marked as completed and moved to stack!\n", current->name);
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

