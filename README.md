# To-Do List Application (Development Progress Repository)

This repository captures the **full development history** of the To-Do List Application for the **CPE112 Data Structures** course.  
It includes initial prototypes, testing files, code experiments, and feature evolution throughout the semester.

---

Notes
	•	This repository is intentionally messier than the final one. It includes failed approaches, debug versions, and backups for reference.
	•	Some functions or files may be partially implemented or deprecated — they are kept for historical value.

---
### Final Projec

Visit the Final Repository for the clean, optimized version with a full feature list.

link : 

---

## Purpose

While the final version of this project is hosted in a separate repository, **this progress repository** serves to:
- Track all stages of design and implementation
- Document the trial-and-error learning process
- Preserve backup versions of core modules
- Reflect on growth in C programming and data structure application

---

## Key Development Phases

### Phase 1: Initial Setup
- Basic terminal input/output  
- Struct definition for `Task`  
- Initial task creation with static arrays

### Phase 2: Linked List Integration
- Dynamic task storage using singly linked list  
- Functions: add, edit, delete, display  

### Phase 3: Stack & Undo Mechanism
- Stack implementation for completed task tracking  
- Undo functionality using stack pop  

### Phase 4: Scheduler & Automation
- Due date comparison and overdue detection  
- Automatic task status updates  
- Day simulation for deadline testing

### Phase 5: File I/O
- Import/export tasks from `.txt` files  
- Backup and recovery logic  

### Phase 6: Search, Tags, and Filters
- Search by keyword, priority, tag, and status  
- Filter tasks by due date range or missing due dates

### Phase 7: Statistics & Views
- Task completion stats  
- Weekly/monthly summaries  
- Enhanced display modes for better readability

---

## Directory Structure
```bash
/backup1/
│
├── fileio.c / .h              # File read/write operations
├── task_management.c / .h     # Core task logic (add/edit/delete)
├── scheduler.c / .h           # Deadline checks and day simulation
├── searchandstat.c / .h       # Search, filters, and statistics
├── main.c                     # Main menu and user interface
├── sample_tasks.txt           # Example input file
├── [old versions]/            # Experimental or backup files
```

---

## How to Compile & Run

```bash
gcc main.c task_management.c scheduler.c searchandstat.c fileio.c -o todo_progress
```
then
```bash
./todo_progress
```

