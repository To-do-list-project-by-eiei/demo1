# 📝 To-Do List Application (Data Structures Project)

This project is a terminal-based To-Do List application written in **C**, created for the **iCPE112 Data Structures** course.  
It helps users manage tasks with scheduling, reminders, priority adjustment, and statistics using key data structures.

---

## 📚 Introduction

In daily life, tasks often vary in priority and urgency. Traditional to-do lists fail to address:
- Deadlines approaching
- Reordering priorities dynamically
- Tracking completed tasks

This project solves those problems by:
- Using a **priority queue** to manage urgent tasks
- Applying a **stack** to track completed items with undo support
- Using **linked lists** to allow dynamic task insertion/removal
- Including **date handling** and reminder logic to notify the user

---
## 📌 Overview

Managing tasks with different levels of urgency is a common challenge.  
This To-Do List application enables:
- Task creation with due dates and priority levels
- Automatic sorting and reminders
- Undo functionality for task completion
- Task statistics and daily tracking

It’s designed to improve productivity, reduce stress, and provide an organized workflow in a **command-line interface**.

---

## 🚀 Features

- ✅ Add, Edit, and Delete Tasks
- ✅ Assign Due Dates and Get Reminders
- ✅ Mark Tasks as Completed (Undo Available)
- ✅ Automatic Priority Adjustment Near Deadlines
- ✅ Simulate Day Changes (Test Time-Based Features)
- ✅ Sort and Filter by Priority, Due Date, Status
- ✅ Task Completion Stats and Daily Logs
- ✅ Export and Backup Support

---

## 🧠 Data Structures Used

| Structure | Purpose |
|-----------|---------|
| **Linked List** | Store tasks dynamically |
| **Stack** | Track completed tasks (Undo / Clear Stack) |
| **Priority Queue** | Sort tasks by urgency |


---


## 📁 File Structure

```bash
📁 ToDoList-DSA/
├── main.c               # Entry point of the program
├── scheduler.h          # Scheduling, reminders, due dates, priority logic
├── task_management.h        # Add/edit/delete/complete task functions
├── searchstat.h        # Search functionality and statistics tracking
├── README.md            # Project documentation
```
---



## 💻 How to Run the Code

### 🧾 Requirements
- A C compiler (`gcc`)
- All header files (`scheduler.h`, `task_management.h`, `searchstats.h`) and `main.c` in the same folder

### ⚙️ Compilation

first
```bash
gcc main.c task_management.c searchandstat.c scheduler.c fileio.c
```
then 
```bash
./todolist
```
the file should run properly

---

###  Sample input

1.
```bash
CompProg, Linked List practice - Practice linked list operations for homework, 1, 30/04/2025
```
2.
```bash
CalII, Integration review - Study integration techniques for upcoming exam, 1, 02/05/2025
```
more sample input on the file sample_import.txt

## expected output
```bash
Name: CompProg [!]URGENT
Description:  Debugging session - Fix errors in C program homework
Status: Pending
Due Date: 30/04/2025
```
```bash
Name: CalII
Description:  Integration review - Study integration techniques for upcoming exam
Status: Pending
Due Date: 02/05/2025
```


## 👨‍👩‍👧‍👦 Group Members

| Name                        | Student ID    | Role / Responsibilities                    |
|-----------------------------|---------------|---------------------------------------------|
| **Kulchaya Paipinij**       | 67070503406   | Search, Data Handling & Statistics (`searchstat.h`) |
| **Chayanit Kuntanarumitkul** | 67070503408   | Scheduling, Reminders & Automation (`scheduler.h`) |
| **Siripitch Chaiyabutra**   | 67070503440   | Task Management & Organization (`task_management.h`) |

> Each member was responsible for both development and documentation in their assigned areas.

---

## 🌟 Acknowledgments

Special thanks to our professor and TAs for guiding us through the concepts of data structures, which helped us turn theory into a working, real-world project.




