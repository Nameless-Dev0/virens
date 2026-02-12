#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ucontext.h>
#include <signal.h>
#include <error.h>
#include <assert.h>
#include "scheduler.h"
#include "task_internal.h"
#include "queue.h"

static volatile sig_atomic_t preempt = 0;
static task_t* current_task;
static queue_t* ready_queue;
static ucontext_t ctx_sched;
static ucontext_t ctx_user; // used to return back to user program upon all tasks completion

static void timing_handler(int sig, siginfo_t* info, void* kernel_saved_ctx);
static void scheduler_start();

ucontext_t* scheduler_ctx(void) { // to control access to ctx_sched from task_internals
    return &ctx_sched;
}

ucontext_t* user_ctx(void) { // to control access to ctx_user from user
    return &ctx_user;
}

scheduler_error_t scheduler_init(task_t** list_of_task_handles, size_t num_of_tasks){
    ctx_sched.uc_stack.ss_sp = malloc(SCHEDULER_STACK_SIZE); // Is freed in task_destroy called by scheduler
    if(!(ctx_sched.uc_stack.ss_sp)){
        perror("Task stack allocation failed");
        return SCHEDULER_STACK_ALLOCATION_FAIL;
    }
    ctx_sched.uc_stack.ss_size = SCHEDULER_STACK_SIZE;
    ctx_sched.uc_stack.ss_flags = 0;

    ctx_sched.uc_link = user_ctx(); // Should actually be set to calling user context, might use a macro to getcontext and set the user's context just exit() for now
    if(getcontext(&ctx_sched) == -1){
        perror("context save failed");
        return SCHEDULER_CONTEXT_SAVE_FAILED;
    }
    makecontext(&ctx_sched, scheduler_start, 0);

    ready_queue = create_queue(list_of_task_handles, num_of_tasks);
    if(!ready_queue){
        perror("ready queue allocation failed");
        return SCHEDULER_READY_Q_ALLOCATION_FAIL;
    }
    for (size_t i = 0; i < num_of_tasks; i++){
        enqueue(ready_queue, list_of_task_handles[i]);
    }
    
    return S_SUCCESS;
}

void timing_handler(int sig, siginfo_t* info, void* kernel_saved_ctx){
    (void)sig; (void)info; // just for gcc -Wall
    current_task->ctx = *((ucontext_t*)kernel_saved_ctx); // fix memory bug caused by overwriting task stack pointer
    current_task->ctx.uc_link = scheduler_ctx();
    preempt = 1;
    setcontext(&ctx_sched); // Is it unsafe... sure, however I couldn't figure another way so it will do for now
}

static void scheduler_start(){

    // init timing signal
    struct sigaction timing_action;
    timing_action.sa_sigaction = &timing_handler;
    timing_action.sa_flags = SA_SIGINFO;
    sigemptyset(&timing_action.sa_mask);
    sigaction(SIGALRM, &timing_action, NULL);

    // Block alarm signal until current_task points to a valid task.
    sigset_t old_mask, mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);

    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    current_task = top(ready_queue);
    sigprocmask(SIG_UNBLOCK, &old_mask, NULL);

    ualarm(TIME_SLICE, TIME_SLICE);
    while(!is_empty(ready_queue)){
        if(preempt){
            preempt = 0;
            rotate(ready_queue);
        }
        else if(current_task->state == COMPLETED){
            sigprocmask(SIG_BLOCK, &mask, &old_mask);
            task_t* finished_task = current_task;
            advance(ready_queue);
            task_destroy(finished_task);

            if(is_empty(ready_queue)){
                sigprocmask(SIG_SETMASK, &old_mask, NULL);
                break;
            }
            current_task = top(ready_queue);
            sigprocmask(SIG_SETMASK, &old_mask, NULL);
            continue;
        }
        sigprocmask(SIG_BLOCK, &mask, &old_mask);
        current_task = top(ready_queue);
        sigprocmask(SIG_UNBLOCK, &old_mask, NULL);
        swapcontext(&ctx_sched, &current_task->ctx);
    }
    ualarm(0, 0);
}

void scheduler_destroy(void){
    // destroy queue, free ctx_sched stack, task_destroy for each task, ...

    destroy_queue(&ready_queue);
    ready_queue = NULL;

    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);

    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    free(ctx_sched.uc_stack.ss_sp);
    ctx_sched.uc_stack.ss_sp = NULL;
    ctx_sched.uc_stack.ss_size = 0;
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

}