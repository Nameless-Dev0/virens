#include <stdio.h>
#include <unistd.h>
#include "virens.h"

void function_A(void){
    for (size_t i = 0; i < 10; i++){
        printf("hello from function A!\n");
        usleep(2*1000000);
    }
}

void function_B(void){
    for (size_t i = 0; i < 10; i++){
        printf("Hello from function B!\n");
        usleep(2*1000000);
    }
}
void function_C(void){
    for (size_t i = 0; i < 10; i++){
        printf("Hello from function C!\n");
        usleep(2*1000000);
    }
}

int main(void){
    task_handle_t taskA = NULL, taskB = NULL, taskC = NULL;
    task_error_t t1 = task_create(&taskA, (void*)function_A, NULL, NULL);
    task_error_t t2  = task_create(&taskB, (void*)function_B, NULL, NULL);
    task_error_t t3  = task_create(&taskC, (void*)function_C, NULL, NULL);
    if(t1 != T_SUCCESS) {return -1;}
    if(t2 != T_SUCCESS) {return -1;}
    if(t3 != T_SUCCESS) {return -1;}
    task_handle_t tasks[] = {taskA, taskB, taskC};
    if(scheduler_init(tasks, sizeof(tasks)/sizeof(task_handle_t)) != S_SUCCESS) {return -1;}

    SCHEDULER_RUN();
    printf("Scheduler finished, returned to user context\n");

    return 0;
}