#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "scheduler.h"
#include "task_management.h"
#include "searchandstat.h" // This is needed for showStats and show_time_stats



void sortTasksByDueDate(task* tasks[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Task with no due date goes to the end
            if (!tasks[j]->due_date_set && tasks[j+1]->due_date_set) {
                // Swap
                task* temp = tasks[j];
                tasks[j] = tasks[j+1];
                tasks[j+1] = temp;
            }
            // Both have due dates, compare them
            else if (tasks[j]->due_date_set && tasks[j+1]->due_date_set) {
                if (compareDates(tasks[j]->duedate, tasks[j+1]->duedate) > 0) {
                    // Swap
                    task* temp = tasks[j];
                    tasks[j] = tasks[j+1];
                    tasks[j+1] = temp;
                }
            }
        }
    }
}

void add(tasklist* list) {
    task* new_task = (task*)malloc(sizeof(task));
    if (!new_task) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Initialize tag-related fields right after allocation
    new_task->tag_count = 0;  // Initialize with no tags

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
    
    // First, update statuses based on current date
    updateTaskStatuses(current, today);

    // Create arrays to store tasks by priority and status
    task* high_priority[100];
    task* medium_priority[100];
    task* low_priority[100];
    task* overdue_tasks[100];
    
    int high_count = 0, medium_count = 0, low_count = 0, overdue_count = 0;
    
    // Organize tasks by priority and status
    while (current) {
        // If task is overdue, add to overdue array regardless of priority
        if (!current->completed && current->status == OVERDUE) {
            overdue_tasks[overdue_count++] = current;
        } 
        // Otherwise, categorize by priority
        else if (!current->completed) {
            switch (current->priority) {
                case 1: high_priority[high_count++] = current; break;
                case 2: medium_priority[medium_count++] = current; break;
                case 3: low_priority[low_count++] = current; break;
            }
        }
        current = current->next;
    }
    
    // Sort overdue tasks by how overdue they are (most overdue first)
    for (int i = 0; i < overdue_count - 1; i++) {
        for (int j = 0; j < overdue_count - i - 1; j++) {
            if (overdue_tasks[j]->due_date_set && overdue_tasks[j+1]->due_date_set) {
                if (compareDates(overdue_tasks[j]->duedate, overdue_tasks[j+1]->duedate) > 0) {
                    // Swap
                    task* temp = overdue_tasks[j];
                    overdue_tasks[j] = overdue_tasks[j+1];
                    overdue_tasks[j+1] = temp;
                }
            }
        }
    }
    
    // Sort each priority group by due date
    sortTasksByDueDate(high_priority, high_count);
    sortTasksByDueDate(medium_priority, medium_count);
    sortTasksByDueDate(low_priority, low_count);
    
    // Display overdue tasks first
    if (overdue_count > 0) {
        printf("\n--- OVERDUE TASKS ---\n");
        for (int i = 0; i < overdue_count; i++) {
            printf("Name: %s [OVERDUE]\n", overdue_tasks[i]->name);
            printf("Description: %s\n", overdue_tasks[i]->description);
            printf("Priority: %d (%s)\n", overdue_tasks[i]->priority, 
                   (overdue_tasks[i]->priority == 1) ? "High" : 
                   (overdue_tasks[i]->priority == 2) ? "Medium" : "Low");
            
            if (overdue_tasks[i]->due_date_set) {
                printf("Due Date: %02d/%02d/%04d (PAST DUE)\n", 
                       overdue_tasks[i]->duedate.day, 
                       overdue_tasks[i]->duedate.month, 
                       overdue_tasks[i]->duedate.year);
            } else {
                printf("Due Date: Not Set\n");
            }
            printf("-------------------------\n");
        }
    }
    
    // Display high priority tasks
    if (high_count > 0) {
        printf("\n--- HIGH PRIORITY TASKS ---\n");
        printf("(Sorted by due date - earliest first)\n");
        for (int i = 0; i < high_count; i++) {
            printf("Name: %s", high_priority[i]->name);
            
            // Show urgent tag for tasks due soon
            if (high_priority[i]->due_date_set && isDateSoon(today, high_priority[i]->duedate, 2)) {
                printf(" [!]URGENT");
            }
            printf("\n");
            
            printf("Description: %s\n", high_priority[i]->description);
            printf("Status: Pending\n");
            
            if (high_priority[i]->due_date_set) {
                printf("Due Date: %02d/%02d/%04d", 
                       high_priority[i]->duedate.day, 
                       high_priority[i]->duedate.month, 
                       high_priority[i]->duedate.year);
                
                // Calculate days until due
                int daysLeft = getDaysBetween(today, high_priority[i]->duedate);
                if (daysLeft == 0) {
                    printf(" (DUE TODAY)");
                } else if (daysLeft == 1) {
                    printf(" (DUE TOMORROW)");
                } else if (daysLeft > 0) {
                    printf(" (%d days left)", daysLeft);
                }
                printf("\n");
            } else {
                printf("Due Date: Not Set\n");
            }
            printf("-------------------------\n");
        }
    } else {
        printf("\n--- HIGH PRIORITY TASKS ---\n");
        printf("No high priority tasks.\n");
        printf("-------------------------\n");
    }
    
    // Display medium priority tasks
    if (medium_count > 0) {
        printf("\n--- MEDIUM PRIORITY TASKS ---\n");
        printf("(Sorted by due date - earliest first)\n");
        for (int i = 0; i < medium_count; i++) {
            printf("Name: %s", medium_priority[i]->name);
            
            // Show urgent tag for tasks due soon
            if (medium_priority[i]->due_date_set && isDateSoon(today, medium_priority[i]->duedate, 2)) {
                printf(" [!]URGENT");
            }
            printf("\n");
            
            printf("Description: %s\n", medium_priority[i]->description);
            printf("Status: Pending\n");
            
            if (medium_priority[i]->due_date_set) {
                printf("Due Date: %02d/%02d/%04d", 
                       medium_priority[i]->duedate.day, 
                       medium_priority[i]->duedate.month, 
                       medium_priority[i]->duedate.year);
                
                // Calculate days until due
                int daysLeft = getDaysBetween(today, medium_priority[i]->duedate);
                if (daysLeft == 0) {
                    printf(" (DUE TODAY)");
                } else if (daysLeft == 1) {
                    printf(" (DUE TOMORROW)");
                } else if (daysLeft > 0) {
                    printf(" (%d days left)", daysLeft);
                }
                printf("\n");
            } else {
                printf("Due Date: Not Set\n");
            }
            printf("-------------------------\n");
        }
    } else {
        printf("\n--- MEDIUM PRIORITY TASKS ---\n");
        printf("No medium priority tasks.\n");
        printf("-------------------------\n");
    }
    
    // Display low priority tasks
    if (low_count > 0) {
        printf("\n--- LOW PRIORITY TASKS ---\n");
        printf("(Sorted by due date - earliest first)\n");
        for (int i = 0; i < low_count; i++) {
            printf("Name: %s", low_priority[i]->name);
            
            // Show urgent tag for tasks due soon
            if (low_priority[i]->due_date_set && isDateSoon(today, low_priority[i]->duedate, 2)) {
                printf(" [!]URGENT");
            }
            printf("\n");
            
            printf("Description: %s\n", low_priority[i]->description);
            printf("Status: Pending\n");
            
            if (low_priority[i]->due_date_set) {
                printf("Due Date: %02d/%02d/%04d", 
                       low_priority[i]->duedate.day, 
                       low_priority[i]->duedate.month, 
                       low_priority[i]->duedate.year);
                
                // Calculate days until due
                int daysLeft = getDaysBetween(today, low_priority[i]->duedate);
                if (daysLeft == 0) {
                    printf(" (DUE TODAY)");
                } else if (daysLeft == 1) {
                    printf(" (DUE TOMORROW)");
                } else if (daysLeft > 0) {
                    printf(" (%d days left)", daysLeft);
                }
                printf("\n");
            } else {
                printf("Due Date: Not Set\n");
            }
            printf("-------------------------\n");
        }
    } else {
        printf("\n--- LOW PRIORITY TASKS ---\n");
        printf("No low priority tasks.\n");
        printf("-------------------------\n");
    }
    
    // Display a summary
    int total_tasks = overdue_count + high_count + medium_count + low_count;
    printf("\n=== SUMMARY ===\n");
    printf("Total Active Tasks: %d\n", total_tasks);
    printf("Overdue: %d\n", overdue_count);
    printf("High Priority: %d\n", high_count);
    printf("Medium Priority: %d\n", medium_count);
    printf("Low Priority: %d\n", low_count);
    
    // Check if there are any urgent tasks due soon
    int urgent_count = 0;
    for (int i = 0; i < high_count; i++) {
        if (high_priority[i]->due_date_set && isDateSoon(today, high_priority[i]->duedate, 2)) {
            urgent_count++;
        }
    }
    for (int i = 0; i < medium_count; i++) {
        if (medium_priority[i]->due_date_set && isDateSoon(today, medium_priority[i]->duedate, 2)) {
            urgent_count++;
        }
    }
    for (int i = 0; i < low_count; i++) {
        if (low_priority[i]->due_date_set && isDateSoon(today, low_priority[i]->duedate, 2)) {
            urgent_count++;
        }
    }
    
    if (urgent_count > 0) {
        printf("Urgent Tasks (Due within 2 days): %d\n", urgent_count);
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
                    break;
                }
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
                    break;
                }
                case 4: {
                    printf("Due Date Options:\n");
                    printf("1. Set/Change due date\n");
                    printf("2. Clear due date\n");
                    printf("Enter choice (1-2): ");
                    
                    int due_date_choice;
                    if (fgets(buffer, sizeof(buffer), stdin) != NULL && sscanf(buffer, "%d", &due_date_choice) == 1) {
                        if (due_date_choice == 1) {
                            // Set or change due date
                            int valid_date = 0;
                            
                            while (!valid_date) {
                                printf("Enter new due date (DD MM YYYY): ");
                                int day, month, year;
                                
                                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                                    if (sscanf(buffer, "%d %d %d", &day, &month, &year) == 3) {
                                        if (isValidDate(day, month, year)) {
                                            current->duedate.day = day;
                                            current->duedate.month = month;
                                            current->duedate.year = year;
                                            current->due_date_set = 1;
                                            valid_date = 1;
                                            printf("Task due date updated.\n");
                                        } else {
                                            printf("Invalid date. Please enter a valid date.\n");
                                        }
                                    } else {
                                        printf("Invalid date format. Please use DD MM YYYY format.\n");
                                    }
                                } else {
                                    printf("Error reading date input. Due date not changed.\n");
                                    break;
                                }
                            }
                        } else if (due_date_choice == 2) {
                            // Clear due date
                            current->due_date_set = 0;
                            printf("Due date cleared.\n");
                        } else {
                            printf("Invalid choice. Due date not changed.\n");
                        }
                    } else {
                        printf("Invalid input. Due date not changed.\n");
                    }
                    break;
                }
                default:
                    printf("Invalid choice. Task not updated.\n");
                    return;
            }
            return;
        }
        current = current->next;
    }
    printf("Task '%s' not found.\n", taskname);
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

void add_tag_to_task(tasklist* list, const char* taskname) {
    task* current = list->head;
    
    // Find the task
    while (current && strcmp(current->name, taskname) != 0) {
        current = current->next;
    }
    
    if (!current) {
        printf("Task '%s' not found.\n", taskname);
        return;
    }
    
    // Check if task already has maximum number of tags
    if (current->tag_count >= MAX_TAGS) {
        printf("This task already has the maximum number of tags (%d).\n", MAX_TAGS);
        printf("Current tags: ");
        for (int i = 0; i < current->tag_count; i++) {
            printf("%s%s", current->tags[i], (i < current->tag_count - 1) ? ", " : "");
        }
        printf("\n");
        
        // Ask if user wants to replace a tag
        printf("Would you like to replace a tag? (y/n): ");
        char choice[5];
        fgets(choice, sizeof(choice), stdin);
        if (choice[0] != 'y' && choice[0] != 'Y') {
            return;
        }
        
        // Show tags with numbers
        printf("Select a tag to replace (1-%d):\n", current->tag_count);
        for (int i = 0; i < current->tag_count; i++) {
            printf("%d. %s\n", i + 1, current->tags[i]);
        }
        
        // Get user choice
        int tag_index;
        char buffer[10];
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &tag_index);
        
        if (tag_index < 1 || tag_index > current->tag_count) {
            printf("Invalid selection. No tags were changed.\n");
            return;
        }
        
        // Get new tag
        printf("Enter new tag: ");
        char new_tag[MAX_TAG_LENGTH];
        fgets(new_tag, sizeof(new_tag), stdin);
        new_tag[strcspn(new_tag, "\n")] = 0;
        
        // Replace tag
        strcpy(current->tags[tag_index - 1], new_tag);
        printf("Tag replaced successfully.\n");
        return;
    }
    
    // Get tag from user
    printf("Enter tag for '%s' (max %d chars): ", taskname, MAX_TAG_LENGTH - 1);
    char new_tag[MAX_TAG_LENGTH];
    fgets(new_tag, sizeof(new_tag), stdin);
    new_tag[strcspn(new_tag, "\n")] = 0;
    
    // Check if tag already exists for this task
    for (int i = 0; i < current->tag_count; i++) {
        if (strcmp(current->tags[i], new_tag) == 0) {
            printf("This task already has the tag '%s'.\n", new_tag);
            return;
        }
    }
    
    // Add tag to task
    strcpy(current->tags[current->tag_count], new_tag);
    current->tag_count++;
    printf("Tag '%s' added to task '%s'.\n", new_tag, taskname);
}

void view_by_tag(tasklist* list, const char* tag) {
    task* current = list->head;
    int found = 0;
    
    printf("\n=== Tasks with Tag '%s' ===\n", tag);
    
    while (current) {
        // Check if task has the specified tag
        for (int i = 0; i < current->tag_count; i++) {
            if (strcmp(current->tags[i], tag) == 0) {
                // Print task details
                printf("Name: %s\n", current->name);
                printf("Description: %s\n", current->description);
                printf("Priority: %d\n", current->priority);
                printf("Status: %s\n", 
                       (current->status == PENDING) ? "Pending" : 
                       (current->status == COMPLETED) ? "Completed" : "Overdue");
                
                if (current->due_date_set) {
                    printf("Due Date: %02d/%02d/%04d\n", 
                           current->duedate.day, current->duedate.month, current->duedate.year);
                } else {
                    printf("Due Date: Not Set\n");
                }
                
                // Print all tags
                printf("Tags: ");
                for (int j = 0; j < current->tag_count; j++) {
                    printf("%s%s", current->tags[j], (j < current->tag_count - 1) ? ", " : "");
                }
                printf("\n-------------------------\n");
                
                found = 1;
                break;  // Found the tag, no need to check other tags for this task
            }
        }
        
        current = current->next;
    }
    
    if (!found) {
        printf("No tasks found with tag '%s'.\n", tag);
    }
}

void sort_by_tag(tasklist* list) {
    // First, get all unique tags from all tasks
    char unique_tags[100][MAX_TAG_LENGTH];  // Assume max 100 unique tags
    int tag_count = 0;
    
    task* current = list->head;
    while (current) {
        for (int i = 0; i < current->tag_count; i++) {
            // Check if this tag is already in our unique_tags array
            int is_unique = 1;
            for (int j = 0; j < tag_count; j++) {
                if (strcmp(current->tags[i], unique_tags[j]) == 0) {
                    is_unique = 0;
                    break;
                }
            }
            
            // If it's a new tag, add it to our array
            if (is_unique) {
                strcpy(unique_tags[tag_count], current->tags[i]);
                tag_count++;
            }
        }
        current = current->next;
    }
    
    // Display tags and let user select one to view
    if (tag_count == 0) {
        printf("No tags found in any tasks.\n");
        return;
    }
    
    printf("\n=== Available Tags ===\n");
    for (int i = 0; i < tag_count; i++) {
        printf("%d. %s\n", i + 1, unique_tags[i]);
    }
    
    printf("Select a tag to view (1-%d): ", tag_count);
    char buffer[10];
    fgets(buffer, sizeof(buffer), stdin);
    int selection;
    sscanf(buffer, "%d", &selection);
    
    if (selection < 1 || selection > tag_count) {
        printf("Invalid selection.\n");
        return;
    }
    
    // View tasks with the selected tag
    view_by_tag(list, unique_tags[selection - 1]);
}

void simplified_view(tasklist* list, date today) {
    task* current = list->head;
    printf("\n=== Simplified Task List ===\n");
    
    if (!current) {
        printf("No tasks available.\n");
        return;
    }
    
    // First, update statuses based on current date
    updateTaskStatuses(current, today);
    
    // Count tasks in each category
    int overdue = 0, urgent = 0, pending = 0, today_count = 0;
    
    current = list->head;
    while (current) {
        if (!current->completed) {
            if (current->status == OVERDUE) {
                overdue++;
            } else if (current->due_date_set && isDateSoon(today, current->duedate, 2)) {
                urgent++;
            } else {
                pending++;
            }
            
            // Count tasks due today
            if (current->due_date_set && 
                current->duedate.day == today.day &&
                current->duedate.month == today.month &&
                current->duedate.year == today.year) {
                today_count++;
            }
        }
        current = current->next;
    }
    
    // Display status summary
    printf("Overdue: %d | Urgent: %d | Due Today: %d | Pending: %d\n\n", 
           overdue, urgent, today_count, pending);
    
    // Create arrays for tasks by priority
    task* priority_tasks[3][100]; // For priorities 1, 2, 3 (up to 100 tasks per priority)
    int priority_counts[3] = {0};
    
    // Collect tasks by priority
    current = list->head;
    while (current) {
        if (!current->completed) {
            int priority_idx = current->priority - 1;
            if (priority_idx >= 0 && priority_idx < 3) {
                priority_tasks[priority_idx][priority_counts[priority_idx]++] = current;
            }
        }
        current = current->next;
    }
    
    // Sort tasks within each priority by due date
    for (int p = 0; p < 3; p++) {
        // Sort by due date (bubble sort)
        for (int i = 0; i < priority_counts[p] - 1; i++) {
            for (int j = 0; j < priority_counts[p] - i - 1; j++) {
                // If first task has no due date or second has earlier due date
                if (!priority_tasks[p][j]->due_date_set && priority_tasks[p][j+1]->due_date_set) {
                    // Swap (tasks without due dates go to the end)
                    task* temp = priority_tasks[p][j];
                    priority_tasks[p][j] = priority_tasks[p][j+1];
                    priority_tasks[p][j+1] = temp;
                }
                // Both have due date, compare them
                else if (priority_tasks[p][j]->due_date_set && priority_tasks[p][j+1]->due_date_set) {
                    if (compareDates(priority_tasks[p][j]->duedate, priority_tasks[p][j+1]->duedate) > 0) {
                        // Swap
                        task* temp = priority_tasks[p][j];
                        priority_tasks[p][j] = priority_tasks[p][j+1];
                        priority_tasks[p][j+1] = temp;
                    }
                }
            }
        }
    }
    
    // Display table header
    printf("%-3s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Status");
    printf("---------------------------------------------------------------\n");
    
    // Print tasks in priority order, and due date order within each priority
    int count = 1;
    
    // First print overdue tasks at the top (regardless of priority)
    current = list->head;
    while (current) {
        if (!current->completed && current->status == OVERDUE) {
            char date_str[15] = "Not Set";
            if (current->due_date_set) {
                sprintf(date_str, "%02d/%02d/%04d", 
                        current->duedate.day, 
                        current->duedate.month, 
                        current->duedate.year);
            }
            
            char priority_str[10];
            switch(current->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            // Add marker for overdue
            char name_with_markers[30] = "";
            strncpy(name_with_markers, current->name, 25);
            strcat(name_with_markers, "!");
            
            printf("%-3d %-25s %-10s %-15s %-10s\n", 
                   count++, name_with_markers, priority_str, date_str, "OVERDUE");
        }
        current = current->next;
    }
    
    // Then print the rest of the tasks by priority and due date
    for (int p = 0; p < 3; p++) {
        for (int i = 0; i < priority_counts[p]; i++) {
            task* t = priority_tasks[p][i];
            
            // Skip overdue tasks (already printed)
            if (t->status == OVERDUE) continue;
            
            char date_str[15] = "Not Set";
            if (t->due_date_set) {
                sprintf(date_str, "%02d/%02d/%04d", 
                        t->duedate.day, 
                        t->duedate.month, 
                        t->duedate.year);
            }
            
            char priority_str[10];
            switch(t->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            // Add markers for urgent tasks
            char name_with_markers[30] = "";
            strncpy(name_with_markers, t->name, 25);
            
            if (t->due_date_set && isDateSoon(today, t->duedate, 2)) {
                strcat(name_with_markers, "*");
            }
            
            printf("%-3d %-25s %-10s %-15s %-10s\n", 
                   count++, name_with_markers, priority_str, date_str, "Pending");
        }
    }
    
    printf("\nLegend: ! = Overdue, * = Urgent (due within 2 days)\n");
}
// int isDateWithinDays(date today, date check_date, int days) {
//     // Calculate total days for both dates (very simplified calculation)
//     int today_days = today.year * 365 + today.month * 30 + today.day;
//     int check_days = check_date.year * 365 + check_date.month * 30 + check_date.day;
    
//     // Check if the date is within the specified range
//     int diff = check_days - today_days;
//     return (diff >= 0 && diff <= days);
// }

void view_weekly_summary(tasklist* list, date today) {
    task* current = list->head;
    int count = 0;
    
    printf("\n=== Tasks Due This Week (%02d/%02d/%04d to %02d/%02d/%04d) ===\n", 
           today.day, today.month, today.year,
           today.day + 7, today.month, today.year); // Simple calculation, not accounting for month/year boundaries
    
    printf("%-3s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Days Left");
    printf("---------------------------------------------------------------\n");
    
    // Create arrays to store tasks for each day of the week
    task* days_tasks[8][50]; // [0] is for today, [1] for tomorrow, etc., allow up to 50 tasks per day
    int days_tasks_count[8] = {0}; // Track the count of tasks for each day
    
    // First pass: collect tasks in arrays by day
    while (current) {
        if (!current->completed && current->due_date_set) {
            int daysDiff = getDaysBetween(today, current->duedate);
            if (daysDiff >= 0 && daysDiff <= 7) {
                days_tasks[daysDiff][days_tasks_count[daysDiff]++] = current;
            }
        }
        current = current->next;
    }
    
    // Second pass: print details of each task, sorted by day
    int task_num = 1;
    
    for (int day = 0; day <= 7; day++) {
        for (int i = 0; i < days_tasks_count[day]; i++) {
            task* t = days_tasks[day][i];
            
            char date_str[15];
            sprintf(date_str, "%02d/%02d/%04d", 
                    t->duedate.day, 
                    t->duedate.month, 
                    t->duedate.year);
            
            char priority_str[10];
            switch(t->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            char days_left[10];
            if (day == 0) {
                strcpy(days_left, "Today");
            } else if (day == 1) {
                strcpy(days_left, "Tomorrow");
            } else {
                sprintf(days_left, "%d days", day);
            }
            
            printf("%-3d %-25s %-10s %-15s %-10s\n", 
                   task_num++, t->name, priority_str, date_str, days_left);
            count++;
        }
    }
    
    if (count == 0) {
        printf("No tasks due this week.\n");
    } else {
        printf("\nTotal: %d tasks due this week\n", count);
    }
    
    // Daily summary MOVED to the bottom
    printf("\nDaily summary:\n");
    printf("Today: %d tasks\n", days_tasks_count[0]);
    printf("Tomorrow: %d tasks\n", days_tasks_count[1]);
    for (int i = 2; i <= 7; i++) {
        printf("In %d days: %d tasks\n", i, days_tasks_count[i]);
    }
}


// void show_combined_stats(task* head, completedstack* stack, date today) {
//     int choice;
//     char buffer[10];
    
//     printf("\n=== Task Statistics View ===\n");
//     printf("1. All-time Statistics\n");
//     printf("2. Weekly Statistics\n");
//     printf("3. Monthly Statistics\n");
//     printf("Enter your choice (1-3): ");
    
//     if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
//         printf("Invalid input. Showing all-time statistics by default.\n");
//         choice = 1;
//     }
    
//     switch(choice) {
//         case 1:
//             showStats(head, stack, today);  // Original all-time stats function
//             break;
//         case 2:
//             show_time_stats(head, stack, today, 0);  // Weekly stats (0 = week)
//             break;
//         case 3:
//             show_time_stats(head, stack, today, 1);  // Monthly stats (1 = month)
//             break;
//         default:
//             printf("Invalid option. Showing all-time statistics.\n");
//             showStats(head, stack, today);
//     }
// }

void view_time_summary(tasklist* list, date today) {
    int choice;
    char buffer[10];
    
    printf("\n=== Time Period Summary ===\n");
    printf("1. Weekly Summary (Next 7 days)\n");
    printf("2. Monthly Summary (Current month)\n");
    printf("Enter your choice (1-2): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
        printf("Invalid input. Showing weekly summary by default.\n");
        choice = 1;
    }
    
    switch(choice) {
        case 1:
            view_weekly_summary(list, today);
            break;
        case 2:
            view_monthly_summary(list, today);
            break;
        default:
            printf("Invalid option. Showing weekly summary.\n");
            view_weekly_summary(list, today);
    }
}

void view_combined(tasklist* list, date today) {
    int choice;
    char buffer[10];
    
    printf("\n=== Task View Options ===\n");
    printf("1. Standard View (All Tasks)\n");
    printf("2. Simplified View (Compact Format)\n");
    printf("3. View by Tag\n");
    printf("Enter your choice (1-3): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
        printf("Invalid input. Using standard view by default.\n");
        choice = 1;
    }
    
    switch(choice) {
        case 1:
            view(list, today);  // Original view function
            break;
        case 2:
            simplified_view(list, today);
            break;
        case 3:
            sort_by_tag(list);  // This function already includes tag selection
            break;
        default:
            printf("Invalid option. Using standard view.\n");
            view(list, today);
    }
}

void view_monthly_summary(tasklist* list, date today) {
    task* current = list->head;
    int count = 0;
    
    // Calculate the end date of the month (simplified, not accounting for different month lengths)
    int days_in_month;
    switch (today.month) {
        case 2: // February
            // Simple leap year check
            if ((today.year % 4 == 0 && today.year % 100 != 0) || today.year % 400 == 0) {
                days_in_month = 29;
            } else {
                days_in_month = 28;
            }
            break;
        case 4: case 6: case 9: case 11: // 30-day months
            days_in_month = 30;
            break;
        default: // 31-day months
            days_in_month = 31;
    }
    
    printf("\n=== Tasks Due This Month (%02d/%04d) ===\n", today.month, today.year);
    
    printf("%-3s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Days Left");
    printf("---------------------------------------------------------------\n");
    
    // Count tasks by week
    int week_count[5] = {0}; // 5 weeks in a month (approximately)
    task* week_tasks[5][50]; // Up to 50 tasks per week
    int week_tasks_count[5] = {0};
    
    // First pass: collect and count tasks by week
    while (current) {
        if (!current->completed && current->due_date_set) {
            // Check if the task is due this month
            if (current->duedate.month == today.month && current->duedate.year == today.year) {
                int daysDiff = getDaysBetween(today, current->duedate);
                if (daysDiff >= 0) {
                    int week = daysDiff / 7;
                    if (week < 5) {
                        week_tasks[week][week_tasks_count[week]++] = current;
                        week_count[week]++;
                    }
                }
            }
        }
        current = current->next;
    }
    
    // Second pass: print task details sorted by week
    int task_num = 1;
    
    for (int week = 0; week < 5; week++) {
        // Sort tasks within each week by priority, then by due date
        for (int i = 0; i < week_tasks_count[week] - 1; i++) {
            for (int j = 0; j < week_tasks_count[week] - i - 1; j++) {
                // Primary sort by priority (1 is highest)
                if (week_tasks[week][j]->priority > week_tasks[week][j+1]->priority) {
                    // Swap
                    task* temp = week_tasks[week][j];
                    week_tasks[week][j] = week_tasks[week][j+1];
                    week_tasks[week][j+1] = temp;
                }
                // Secondary sort by due date (if same priority)
                else if (week_tasks[week][j]->priority == week_tasks[week][j+1]->priority &&
                        compareDates(week_tasks[week][j]->duedate, week_tasks[week][j+1]->duedate) > 0) {
                    // Swap
                    task* temp = week_tasks[week][j];
                    week_tasks[week][j] = week_tasks[week][j+1];
                    week_tasks[week][j+1] = temp;
                }
            }
        }
        
        // Print tasks for this week
        for (int i = 0; i < week_tasks_count[week]; i++) {
            task* t = week_tasks[week][i];
            
            char date_str[15];
            sprintf(date_str, "%02d/%02d/%04d", 
                    t->duedate.day, 
                    t->duedate.month, 
                    t->duedate.year);
            
            char priority_str[10];
            switch(t->priority) {
                case 1: strcpy(priority_str, "High"); break;
                case 2: strcpy(priority_str, "Medium"); break;
                case 3: strcpy(priority_str, "Low"); break;
                default: strcpy(priority_str, "Unknown");
            }
            
            int daysDiff = getDaysBetween(today, t->duedate);
            char days_left[10];
            if (daysDiff == 0) {
                strcpy(days_left, "Today");
            } else if (daysDiff == 1) {
                strcpy(days_left, "Tomorrow");
            } else {
                sprintf(days_left, "%d days", daysDiff);
            }
            
            printf("%-3d %-25s %-10s %-15s %-10s\n", 
                   task_num++, t->name, priority_str, date_str, days_left);
            count++;
        }
    }
    
    if (count == 0) {
        printf("No tasks due this month.\n");
    } else {
        printf("\nTotal: %d tasks due this month\n", count);
    }
    
    // Weekly summary MOVED to the bottom
    printf("\nWeekly summary:\n");
    printf("This week (next 7 days): %d tasks\n", week_count[0]);
    printf("Next week (8-14 days): %d tasks\n", week_count[1]);
    printf("Week 3 (15-21 days): %d tasks\n", week_count[2]);
    printf("Week 4 (22-28 days): %d tasks\n", week_count[3]);
    if (days_in_month > 28) {
        printf("End of month (29+ days): %d tasks\n", week_count[4]);
    }
}

// void text_converter(const char* input_text, tasklist* list) {
//     if (!input_text || strlen(input_text) == 0) {
//         printf("Please enter the text to convert to tasks:\n");
//         char buffer[1000];
//         fgets(buffer, sizeof(buffer), stdin);
//         input_text = buffer;
//     }
    
//     // Make a copy of the input text to avoid modifying the original
//     char text_copy[5000];
//     strncpy(text_copy, input_text, sizeof(text_copy) - 1);
//     text_copy[sizeof(text_copy) - 1] = '\0';
    
//     // Split text by lines
//     char* line = strtok(text_copy, "\n");
//     int tasks_added = 0;
    
//     while (line) {
//         // Skip empty lines
//         if (strlen(line) == 0) {
//             line = strtok(NULL, "\n");
//             continue;
//         }
        
//         // Basic parsing: Each line becomes a task
//         task* new_task = (task*)malloc(sizeof(task));
//         if (!new_task) {
//             printf("Memory allocation failed.\n");
//             return;
//         }
        
//         // Initialize tag count
//         new_task->tag_count = 0;
        
//         // Extract task name (use first 80 chars or up to colon if present)
//         char* colon = strchr(line, ':');
//         if (colon) {
//             int name_len = colon - line;
//             if (name_len > 99) name_len = 99;
//             strncpy(new_task->name, line, name_len);
//             new_task->name[name_len] = '\0';
            
//             // Description is everything after the colon
//             if (strlen(colon + 1) > 0) {
//                 strncpy(new_task->description, colon + 1, sizeof(new_task->description) - 1);
//                 new_task->description[sizeof(new_task->description) - 1] = '\0';
//                 // Trim leading spaces in description
//                 char* desc_start = new_task->description;
//                 while (*desc_start == ' ' || *desc_start == '\t') {
//                     desc_start++;
//                 }
//                 if (desc_start != new_task->description) {
//                     memmove(new_task->description, desc_start, strlen(desc_start) + 1);
//                 }
//             } else {
//                 strcpy(new_task->description, "");
//             }
//         } else {
//             // No colon found, use the whole line as name
//             strncpy(new_task->name, line, sizeof(new_task->name) - 1);
//             new_task->name[sizeof(new_task->name) - 1] = '\0';
//             strcpy(new_task->description, "");
//         }
        
//         // Trim trailing spaces in name
//         int len = strlen(new_task->name);
//         while (len > 0 && (new_task->name[len-1] == ' ' || new_task->name[len-1] == '\t')) {
//             new_task->name[--len] = '\0';
//         }
        
//         // Check for duplicate task name
//         if (isTaskNameDuplicate(list, new_task->name)) {
//             // Append a unique identifier to the name
//             char unique_name[100];
//             sprintf(unique_name, "%s (%d)", new_task->name, tasks_added + 1);
//             strcpy(new_task->name, unique_name);
//         }
        
//         // Set default values
//         new_task->priority = 2;  // Medium priority by default
//         new_task->due_date_set = 0;  // No due date
//         new_task->completed = 0;  // Not completed
//         new_task->status = PENDING;  // Pending status
        
//         // Look for priority indicators
//         if (strstr(line, "!") || 
//             strstr(line, "high") || strstr(line, "HIGH") || 
//             strstr(line, "urgent") || strstr(line, "URGENT")) {
//             new_task->priority = 1;  // High priority
//         } else if (strstr(line, "low") || strstr(line, "LOW") || 
//                    strstr(line, "later") || strstr(line, "LATER")) {
//             new_task->priority = 3;  // Low priority
//         }
        
//         // Add to list
//         new_task->next = list->head;
//         list->head = new_task;
//         tasks_added++;
        
//         // Get next line
//         line = strtok(NULL, "\n");
//     }
    
//     printf("%d tasks added from text.\n", tasks_added);
// }

