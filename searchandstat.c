#include <stdio.h>
#include <string.h>
#include <time.h>
#include "searchandstat.h"

void search(task* head, const char* keyword){
    task* node = head;
    int found = 0;
    while(node){
        if(strstr(node->name, keyword) || strstr(node->description, keyword)){
            printf("Found:\nName:%s\nDesc:%s\nPriority:%d\nDue:%d/%d/%d\nStatus:%s\n",
                node->name, node->description, node->priority,
                node->duedate.day, node->duedate.month, node->duedate.year,
                node->complete ? "Completed" : "Pending");
            found = 1;
        }
        node = node->next;
    }
    if(!found)
        printf("'%s' not found.\n", keyword);
}

// void exportTasks(task* head, const char* filename){
//     FILE* file = fopen(filename, "w");
//     if(!file){
//         printf("Can't open %s\n", filename);
//         return;
//     }
//     task* ptr = head;
//     while(ptr){
//         fprintf(file, "%s,%s,%d,%d-%d-%d,%d\n",
//             ptr->name, ptr->description, ptr->priority,
//             ptr->duedate.year, ptr->duedate.month, ptr->duedate.day,
//             ptr->complete);
//         ptr = ptr->next;
//     }
//     fclose(file);
//     printf("Saved to %s\n", filename);
// }

void stat(task* head){
    int total = 0, done = 0, pending = 0;
    task* p = head;
    while(p){
        total++;
        if(p->complete) done++;
        else pending++;
        p = p->next;
    }
    printf("Stats:\nTotal: %d  Done: %d  Pending: %d\n", total, done, pending);
}

void doneToday(task* head){
    time_t now = time(0);
    struct tm *today = localtime(&now);
    char todaystr[11];
    sprintf(todaystr, "%04d-%02d-%02d", today->tm_year+1900, today->tm_mon+1, today->tm_mday);

    int count = 0;
    task* cur = head;
    while(cur){
        if(cur->complete){ // You could extend this if you track completed date
            printf("Done Today: Name: %s\n", cur->name);
            count++;
        }
        cur = cur->next;
    }
    if(count==0)
        printf("No task done today.\n");
    else
        printf("Total done today: %d\n", count);
}
