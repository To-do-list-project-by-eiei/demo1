#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "scheduler.h"
#include "task_management.h"
#include "searchandstat.h" 


/*
sortTasksByDueDate() - Bubble sorts tasks by due date
 - Time: O(n²), Space: O(1)
 - Sample Case:
    Input: [Task3(15/05), Task1(10/05), Task2(no date)]
    Output: [Task1(10/05), Task3(15/05), Task2(no date)]
 */
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


/*
add() - Adds a new task to the linked list
 - Steps: 1) Allocate memory 2) Get user input 3) Validate 4) Insert at head
 - Time: O(n), Space: O(1)
 - Example: add(&tasks) -> prompts for task details -> adds to list

 Sample Case:
 -Input: 
    Name: "Complete Assignment"
    Description: "Finish data structures homework"
    Priority: 1 (High)
    Due Date: 10 05 2025
 -Output:
    Task added successfully!
    List now contains: "Complete Assignment" -> [previous tasks]
 */
void add(tasklist* list) {
    task* new_task = (task*)malloc(sizeof(task));
    if (!new_task) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Initialize tag-related fields right after allocation
    new_task->tag_count = 0;  // Initialize with no tags

    char task_name[100];
    bool is_valid_name = false; 

    do {
        printf("Enter task name: ");
        if (fgets(task_name, sizeof(task_name), stdin) == NULL) {
             // Handle potential input error 
             printf("Error reading input.\n");
             free(new_task); // Clean up allocated memory
             return;
        }
        task_name[strcspn(task_name, "\n")] = 0; // Remove trailing newline

        // Input Validation : Start 
        is_valid_name = true; 
        bool contains_only_whitespace = true;

        // 1. Check if the string is empty
        if (task_name[0] == '\0') {
            printf("Error: Task name cannot be empty. Please enter a valid name.\n");
            is_valid_name = false;
        } else {
            // 2. Check if the string contains only whitespace
            for (int i = 0; task_name[i] != '\0'; i++) {
                // Check if the character is not whitespace
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
        // Input Validation : End 

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
  
    fgets(new_task->description, sizeof(new_task->description), stdin);
    new_task->description[strcspn(new_task->description, "\n")] = 0;

    printf("Enter priority (1-High, 2-Medium, 3-Low): ");
    
    char buffer[20];
    int priority_input = 0;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%d", &priority_input) == 1) {
             new_task->priority = priority_input;
        } else {
            printf("Invalid priority input. Setting to Medium (2).\n");
            new_task->priority = 2;
        }
    } else {
        printf("Error reading priority input. Setting to Medium (2).\n");
        new_task->priority = 2; 
    }
   

    // Validate priority range
    if (new_task->priority < 1 || new_task->priority > 3) {
        printf("Invalid priority value. Setting to Medium (2).\n");
        new_task->priority = 2;
    }

    // Get due date
    printf("Enter due date (DD MM YYYY): ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        int day, month, year;
        if (sscanf(buffer, "%d %d %d", &day, &month, &year) == 3) {
            // Validate date
            if (isValidDate(day, month, year)) {
                new_task->duedate.day = day;
                new_task->duedate.month = month;
                new_task->duedate.year = year;
                new_task->due_date_set = 1;
            } else {
                printf("Invalid date (Day: %d, Month: %d, Year: %d). Due date not set.\n",
                       day, month, year);
                new_task->due_date_set = 0;
            }
        } else {
            printf("Invalid date format. Due date not set.\n");
            new_task->due_date_set = 0;
        }
    } else {
        printf("Error reading date input. Due date not set.\n");
        new_task->due_date_set = 0;
    }

    new_task->completed = 0;
    new_task->status = PENDING;  
    new_task->next = list->head;
    list->head = new_task;

    printf("Task added successfully!\n");
}


/*
isTaskNameDuplicate() - Checks if task name already exists
 - Time: O(n), Space: O(1)
 - Sample Case:
    Input: 
      List: ["Study", "Project", "Meeting"]
      Name to check: "Study"
    Output: 
      Returns 1 (duplicate found)
 */
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


/*
view() - Displays tasks sorted by priority and due date
 - Time: O(n²), Space: O(n)
 - Sample Case:
    Input: List with 3 tasks of different priorities
    Output:
      --- HIGH PRIORITY TASKS ---
      Name: Urgent Project [!]URGENT
      Due Date: 03/05/2025
      -------------------------
      --- MEDIUM PRIORITY TASKS ---
      Name: Regular Task
      Due Date: 10/05/2025
      -------------------------
 */
void view(tasklist* list, date today) {
    task* current = list->head;
    printf("\n=== Task List ===\n");
    
    if (!current) {
        printf("No tasks available.\n");
        return;
    }
    
    // update statuses based on current date
    updateTaskStatuses(current, today);

    // Create arrays to store tasks by priority and status
    task* high_priority[100];
    task* medium_priority[100];
    task* low_priority[100];
    task* overdue_tasks[100];
    
    int high_count = 0, medium_count = 0, low_count = 0, overdue_count = 0;
    
    // Organize tasks by priority and status
    while (current) {
        // If task is overdue -> add to overdue array regardless of priority
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
    
    // medium priority tasks
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
    
    // low priority tasks
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
    
    // summary
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


/*
edit() - Modifies existing task details
 - Time: O(n), Space: O(1)
 - Sample Case:
    Input:
      Task name: "Essay"
      Choice: 3 (Edit priority)
      New priority: 1
    Output:
      Task priority updated.
 */
void edit(tasklist* list, const char* taskname) {
    task* current = list->head;
    while (current) {
        if (strcmp(current->name, taskname) == 0) {
            int choice;
            char buffer[20]; 

            printf("Editing task: %s\n", current->name);
            printf("Choose what to edit:\n");
            printf("1. Name\n");
            printf("2. Description\n");
            printf("3. Priority\n");
            printf("4. Due Date\n");
            printf("Enter your choice (1-4): ");

            
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choice) != 1) {
                printf("Invalid choice input. Aborting edit.\n");
                return;
            }

            switch (choice) {
                case 1: {
                    char new_name[100];
                    bool is_duplicate; 
                    bool format_valid_and_not_same; 

                    do {
                        printf("Enter new task name: ");
                        if (fgets(new_name, sizeof(new_name), stdin) == NULL) {
                            printf("Error reading input. Aborting name change.\n");
                            return; 
                        }
                        new_name[strcspn(new_name, "\n")] = 0; 

                        // Input Validation : Start 
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
                        //Input Validation : End 

                        if (!is_valid_format) {
                             is_duplicate = true; // Force loop repeat if format is bad
                             format_valid_and_not_same = false;
                             continue; 
                        }

                        // 3. Check if the new name is the same as the current one
                        if (strcmp(new_name, current->name) == 0) {
                            printf("The new name is the same as the current name. No change needed.\n");
                             
                            is_duplicate = false; 
                            format_valid_and_not_same = false; 
                            break; 
                        }

                        // 4. Check if it duplicates *another* existing task name
                        is_duplicate = isTaskNameDuplicate(list, new_name);
                        if (is_duplicate) {
                            printf("Error: A task with this name already exists. Please choose a different name.\n");
                            format_valid_and_not_same = false; 
                        } else {
                           
                            format_valid_and_not_same = true; 
                        }

                    } while (is_duplicate || !format_valid_and_not_same);

                    // Only copy if the loop finished with a valid, different, non-duplicate name
                    if (format_valid_and_not_same) {
                         strcpy(current->name, new_name);
                         printf("Task name updated.\n"); 
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


/*
complete() - Moves task from active list to completed stack
 - Time: O(n), Space: O(1)
 - Sample Case:
    Input: Task name: "Submit Report"
    Before: 
      Active List: "Submit Report" -> "Task 2" -> NULL
      Stack: [empty]
    After:
      Active List: "Task 2" -> NULL
      Stack: "Submit Report" -> NULL
    Output: "Task 'Submit Report' marked as completed!"
 */
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

/*
undoCompleted() - Restores last completed task to active list
 - Time: O(1), Space: O(1)
 - Sample Case:
    Before:
      Active: ["Task A"] -> NULL
      Stack: ["Completed Task"] -> NULL
    After:
      Active: ["Completed Task"] -> ["Task A"] -> NULL
      Stack: NULL
    Output: "Last completed task restored to the list."
 */
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

/*
deleteTask() - Permanently removes task from list
 - Time: O(n), Space: O(1)
 - Sample Case:
    Input: Task name: "Old Task"
    Before: ["Task 1"] -> ["Old Task"] -> ["Task 3"]
    After: ["Task 1"] -> ["Task 3"]
    Output: "Task deleted."
 */
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

/*
sortTasks() - Sorts tasks by priority (insertion sort)
 - Time: O(n²), Space: O(1)
 - Example: sortTasks(&tasks) -> rearranges list: High->Medium->Low priority
 */
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

/*
progress() - Shows completion statistics
 - Time: O(n), Space: O(1)
 - Example: progress(&tasks, &stack) -> "Progress: 5 completed out of 10 (50%)"
 */
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

/*
freeTasks() - Frees all memory in task list
 - Time: O(n), Space: O(1)
 - Example: freeTasks(&tasks) -> deallocates all task nodes
 */
void freeTasks(tasklist* list) {

    task* current = list->head;
    
    while (current) {
    
    task* temp = current;
    
    current = current->next;
    
    free(temp);
    
    }
    
    }
    

/*
freeStack() - Frees all memory in completed stack
 - Time: O(n), Space: O(1)
 - Example: freeStack(&stack) -> deallocates all stack nodes and tasks
 */
void freeStack(completedstack* stack) {
    stacknode* current = stack->top;
    while (current) {
        stacknode* temp = current;
        current = current->next;
        
        if (temp->task_data) {
             free(temp->task_data); // Free the actual task struct memory
        }
        free(temp);            // Free the stack node 
    }
    stack->top = NULL; // Explicitly set top to NULL
}

/*
initQueue() - Initializes empty queue
 - Time: O(1), Space: O(1)
 - Example: initQueue(&queue) -> front=NULL, rear=NULL
 */
void initQueue(taskqueue* q) {
    q->front = q->rear = NULL;
}


/*
enqueue() - Adds task to queue rear
 - Time: O(1), Space: O(1)
 - Sample Case:
    Input: Task to add
    Before: Queue: [Task1] -> [Task2]
    After: Queue: [Task1] -> [Task2] -> [NewTask]
 */
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

/*
dequeue() - Removes task from queue front
 - Time: O(1), Space: O(1)
 - Sample Case:
    Before: Queue: [Task1] -> [Task2] -> [Task3]
    After: Queue: [Task2] -> [Task3]
    Output: Returns Task1
 */
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

/*
isQueueEmpty() - Checks if queue is empty
 - Time: O(1), Space: O(1)
 - Example: isQueueEmpty(&queue) -> returns 1 if empty, 0 if not
 */
int isQueueEmpty(taskqueue* q) {
    return q->front == NULL;
}


/*
freeQueue() - Deallocates queue memory
 - Time: O(n), Space: O(1)
 - Example: freeQueue(&queue) -> frees all queue nodes
 */
void freeQueue(taskqueue* q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
}

/*
add_tag_to_task() - Adds tag to task (max 5 tags)
 - Time: O(n), Space: O(1)
 - Sample Case:
    Input:
      Task name: "Research Paper"
      Choice: 2 (Create new tag)
      New tag: "urgent"
    Output:
      "Tag 'urgent' added to task 'Research Paper'."
 */
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
    
    // Collect all existing tags from all tasks
    char unique_tags[100][MAX_TAG_LENGTH];  
    int tag_count = 0;
    
    task* all_tasks = list->head;
    while (all_tasks) {
        for (int i = 0; i < all_tasks->tag_count; i++) {
           
            int is_unique = 1;
            for (int j = 0; j < tag_count; j++) {
                if (strcmp(all_tasks->tags[i], unique_tags[j]) == 0) {
                    is_unique = 0;
                    break;
                }
            }
            
            // If it's a new tag, add it to our array
            if (is_unique) {
                strcpy(unique_tags[tag_count], all_tasks->tags[i]);
                tag_count++;
            }
        }
        all_tasks = all_tasks->next;
    }
    
    // Show current tags on the task
    if (current->tag_count > 0) {
        printf("\nCurrent tags on this task: ");
        for (int i = 0; i < current->tag_count; i++) {
            printf("%s%s", current->tags[i], (i < current->tag_count - 1) ? ", " : "");
        }
        printf("\n");
    }
    
    
    printf("\n=== Add Tag to Task '%s' ===\n", taskname);
    if (tag_count > 0) {
        printf("1. Choose from existing tags\n");
        printf("2. Create a new tag\n");
        printf("Enter your choice (1-2): ");
        
        int choice;
        char buffer[10];
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &choice);
        
        if (choice == 1) {
            // Show existing tags
            printf("\n=== Available Tags ===\n");
            for (int i = 0; i < tag_count; i++) {
                printf("%d. %s\n", i + 1, unique_tags[i]);
            }
            
            printf("Select a tag (1-%d): ", tag_count);
            int tag_selection;
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%d", &tag_selection);
            
            if (tag_selection < 1 || tag_selection > tag_count) {
                printf("Invalid selection. No tag added.\n");
                return;
            }
            
            // Check if the selected tag already exists on this task
            char* selected_tag = unique_tags[tag_selection - 1];
            for (int i = 0; i < current->tag_count; i++) {
                if (strcmp(current->tags[i], selected_tag) == 0) {
                    printf("This task already has the tag '%s'.\n", selected_tag);
                    return;
                }
            }
            
            // Add the selected tag to the task
            strcpy(current->tags[current->tag_count], selected_tag);
            current->tag_count++;
            printf("Tag '%s' added to task '%s'.\n", selected_tag, taskname);
        }
        else if (choice == 2) {
            // Create new tag
            printf("Enter new tag (max %d chars): ", MAX_TAG_LENGTH - 1);
            char new_tag[MAX_TAG_LENGTH];
            fgets(new_tag, sizeof(new_tag), stdin);
            new_tag[strcspn(new_tag, "\n")] = 0;
            
            // Validate tag name
            if (strlen(new_tag) == 0) {
                printf("Tag name cannot be empty.\n");
                return;
            }
            
            // Check if this tag already exists on this task
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
        else {
            printf("Invalid choice. No tag added.\n");
        }
    }
    else {
        // No existing tags, prompt for new tag
        printf("No existing tags found in the system.\n");
        printf("Enter new tag (max %d chars): ", MAX_TAG_LENGTH - 1);
        char new_tag[MAX_TAG_LENGTH];
        fgets(new_tag, sizeof(new_tag), stdin);
        new_tag[strcspn(new_tag, "\n")] = 0;
        
        // Validate tag name
        if (strlen(new_tag) == 0) {
            printf("Tag name cannot be empty.\n");
            return;
        }
        
        // Add tag to task
        strcpy(current->tags[current->tag_count], new_tag);
        current->tag_count++;
        printf("Tag '%s' added to task '%s'.\n", new_tag, taskname);
    }
}

/*
view_by_tag() - Shows all tasks with specific tag
 - Time: O(n), Space: O(1)
 - Example: view_by_tag(&tasks, "urgent") -> lists all tasks tagged "urgent"
 */
void view_by_tag(tasklist* list, const char* tag) {
    task* current = list->head;
    int found = 0;
    
    // Get today's date and update task statuses
    date today = getToday();
    updateTaskStatuses(list->head, today);
    
    printf("\n=== Tasks with Tag '%s' ===\n", tag);
    
    while (current) {
        // Check if task has the specified tag
        for (int i = 0; i < current->tag_count; i++) {
            if (strcmp(current->tags[i], tag) == 0) {
                // Print task details
                printf("Name: %s\n", current->name);
                printf("Description: %s\n", current->description);
                printf("Priority: %d\n", current->priority);
                
                // Show correct status based on updated information
                if (current->completed) {
                    printf("Status: Completed\n");
                } else if (current->status == OVERDUE) {
                    printf("Status: Overdue\n");
                } else {
                    printf("Status: Pending\n");
                }
                
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

/*
sort_by_tag() - Lists all tags and shows tasks for selected tag
 - Time: O(n²), Space: O(n)
 - Example: sort_by_tag(&tasks) -> shows tag menu, then tasks for chosen tag
 */
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
    
  
    view_by_tag(list, unique_tags[selection - 1]);
}

/*
simplified_view() - Shows compact task list with status summary
 - Time: O(n²), Space: O(n)
 - Sample Case:
    Input: List with various tasks
    Output:
      Overdue: 1 | Urgent: 2 | Due Today: 0 | Pending: 5
      
      #   Name                Priority    Due Date       Status
      --------------------------------------------------------
      1   Overdue Assignment! High        15/04/2025     OVERDUE
      2   Urgent Essay*       High        03/05/2025     Pending
      
      Legend: ! = Overdue, * = Urgent (due within 2 days)
 */
void simplified_view(tasklist* list, date today) {
    task* current = list->head;
    printf("\n=== Simplified Task List ===\n");
    
    if (!current) {
        printf("No tasks available.\n");
        return;
    }
    
    //  update statuses based on current date
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
    task* priority_tasks[3][100]; 
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
    
 
    printf("%-5s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Status");
    printf("--------------------------------------------------------------------\n");
    

    int count = 1;
    

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
            
            
            char name_with_markers[30] = "";
            strncpy(name_with_markers, current->name, 25);
            strcat(name_with_markers, "!");
            
            printf("%-5d %-25s %-10s %-15s %-10s\n", 
                   count++, name_with_markers, priority_str, date_str, "OVERDUE");
        }
        current = current->next;
    }
    
   
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
            
           
            char name_with_markers[30] = "";
            strncpy(name_with_markers, t->name, 25);
            
            if (t->due_date_set && isDateSoon(today, t->duedate, 2)) {
                strcat(name_with_markers, "*");
            }
            
            printf("%-5d %-25s %-10s %-15s %-10s\n", 
                   count++, name_with_markers, priority_str, date_str, "Pending");
        }
    }
    
    printf("\nLegend: ! = Overdue, * = Urgent (due within 2 days)\n");
}


/*
view_weekly_summary() - Shows tasks due in next 7 days
 - Time: O(n²), Space: O(n)
 - Sample Case:
    Input: Current date: 02/05/2025
    Output:
      === Tasks Due This Week (02/05/2025 to 09/05/2025) ===
      #   Name            Priority   Due Date    Days Left
      1   Assignment      High       05/05/2025  3 days
      2   Meeting Prep    Medium     07/05/2025  5 days
      
      Daily summary:
      Today: 0 tasks
      Tomorrow: 1 tasks
 */
void view_weekly_summary(tasklist* list, date today) {
    task* current = list->head;
    int count = 0;
    
    printf("\n=== Tasks Due This Week (%02d/%02d/%04d to %02d/%02d/%04d) ===\n", 
           today.day, today.month, today.year,
           today.day + 7, today.month, today.year); 
    
    printf("%-5s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Days Left");
    printf("---------------------------------------------------------------\n");
    
    // Create arrays to store tasks for each day of the week
    task* days_tasks[8][50];
    int days_tasks_count[8] = {0}; 
    
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
            
            printf("%-5d %-25s %-10s %-15s %-10s\n", 
                   task_num++, t->name, priority_str, date_str, days_left);
            count++;
        }
    }
    
    if (count == 0) {
        printf("No tasks due this week.\n");
    } else {
        printf("\nTotal: %d tasks due this week\n", count);
    }
    
    
    printf("\nDaily summary:\n");
    printf("Today: %d tasks\n", days_tasks_count[0]);
    printf("Tomorrow: %d tasks\n", days_tasks_count[1]);
    for (int i = 2; i <= 7; i++) {
        printf("In %d days: %d tasks\n", i, days_tasks_count[i]);
    }
}




/*
view_time_summary() - Menu for weekly/monthly summary
 - Time: O(n²), Space: O(n)
 - Example: view_time_summary(&tasks, today) -> choice 1=weekly, 2=monthly
 */
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


/*
view_combined() - Menu for standard/simplified/tag view
 - Time: O(n²), Space: O(n)
 - Example: view_combined(&tasks, today) -> shows menu, calls chosen view
 */
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
            view(list, today);  
            break;
        case 2:
            simplified_view(list, today);
            break;
        case 3:
            sort_by_tag(list);  
            break;
        default:
            printf("Invalid option. Using standard view.\n");
            view(list, today);
    }
}

/*
view_monthly_summary() - Shows tasks due this month
 - Time: O(n²), Space: O(n)
 - Example: view_monthly_summary(&tasks, today) -> lists tasks for current month
 */
void view_monthly_summary(tasklist* list, date today) {
    task* current = list->head;
    int count = 0;
    
    
    int days_in_month;
    switch (today.month) {
        case 2: 
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
    
    printf("%-5s %-25s %-10s %-15s %-10s\n", "#", "Name", "Priority", "Due Date", "Days Left");
    printf("---------------------------------------------------------------\n");
    
    // Count tasks by week
    int week_count[5] = {0}; // 5 weeks in a month 
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
    
    
    printf("\nWeekly summary:\n");
    printf("This week (next 7 days): %d tasks\n", week_count[0]);
    printf("Next week (8-14 days): %d tasks\n", week_count[1]);
    printf("Week 3 (15-21 days): %d tasks\n", week_count[2]);
    printf("Week 4 (22-28 days): %d tasks\n", week_count[3]);
    if (days_in_month > 28) {
        printf("End of month (29+ days): %d tasks\n", week_count[4]);
    }
}
