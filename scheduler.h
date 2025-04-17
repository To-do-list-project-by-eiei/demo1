#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define HIGH 1
#define MEDIUM 2
#define LOW 3

typedef struct {
    int day;
    int month;
    int year;
} date ;

typedef struct task {
    char name[100];
    char description[255];
    date duedate;
    int priority;
    int completed;
    struct task *next;
} task;

/*
tasks == linked list
completed task == stack
priority == priority queue
reminder&date == struct
*/

//compare date ( recent and deadline )
int comparedate(date day1, date day2){
    if(day1.year != day2.year){
        return day1.year - day2.year;
    }
    if(day1.month != day2.month){
        return day1.month - day2.month ;
    }
    return day1.day - day2.day;
}

//simulate date (manually setting)
date gettoday(){
    date today;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    today.day = tm.tm_mday;
    today.month = tm.tm_mon + 1;
    today.year = tm.tm_year + 1900;
    return today;

}

//set due date
void setduedate(task* Task, int day, int month, int year){
    Task->duedate.day = day;
    Task->duedate.month = month;
    Task->duedate.year = year;
}

//reminder 
void checkreminder(task *head){
    date today = gettoday();
    task* temp = head;
    printf("reminder check\n");

    while (temp != NULL){
        if(!temp->completed){
            int dayleft = comparedate(temp->duedate , today);
            if(dayleft < 0){
                printf("OVERDUE! : %s (due date : %d/%d/%d )\n", temp->name ,temp->duedate.day, temp->duedate.month, temp->duedate.year );

            } else if(dayleft <= 1){
                printf("DUE SOON! : %s (due date : %d/%d/%d )\n", temp->name ,temp->duedate.day, temp->duedate.month, temp->duedate.year );
            }
        }

        temp = temp->next;
    }
}

//simulate day change ( new simulation day )
void simulatedaychange(task* head, date newdate){
    printf("simulated date : %d/%d/%d\n", newdate.day, newdate.month, newdate.year);
    task* temp = head;
    while(temp != NULL){
        if(!temp->completed && comparedate(temp->duedate, newdate) < 0){
            printf("OVERDUE! : %s (due date : %d/%d/%d )\n", temp->name ,temp->duedate.day, temp->duedate.month, temp->duedate.year );
        }
        temp = temp->next;
    }
}

//adjust priority for near deadline task
void adjustpriority(task* head , date today){
    task* temp = head;
    while (temp != NULL){
        if( !temp->completed){
            int dayleft = comparedate(temp->duedate, today);
            if(dayleft <= 1 && temp->priority > HIGH){
                printf("increase priority of [%s] from %d to %d\n", temp->name, temp->priority, temp->priority -1);
                temp->priority--;
            }
        }
        temp = temp->next;
    }
}

//clear completed tasks from stack (using linked list)
void clearcompletedtask(task** completedstack){
    char confirm;
    printf("clear completed tasks? (y/n): ");
    scanf(" %c", &confirm);
    if(confirm == 'y' || confirm == 'Y'){
        task* temp;
        while (*completedstack != NULL){
            temp = *completedstack;
            *completedstack = (*completedstack)->next;
            free(temp);
        }
        printf("--completed tasks cleared--\n");
    } else {
        printf("--cancelled. completed tasks not clear--\n");
    }
}

#endif