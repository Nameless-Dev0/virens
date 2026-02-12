#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <ucontext.h>
#include "util.h"

#define SCHEDULER_STACK_SIZE 8912
#define TIME_SLICE 100000 // 10ms

typedef struct task task_t;

ucontext_t* scheduler_ctx(void);
ucontext_t* user_ctx(void);
scheduler_error_t scheduler_init(task_t** list_of_task_handles, size_t num_of_tasks);
void scheduler_destroy() __attribute__((unused));

/*
    - Periodic timer using SIGALRM and sigaction, signal handler with the SIGINFO flag to get context of task right before SIGALRM.
    - Context switch by swapcontext to scheduler in signal handler, then in the scheduler reset the timer and swapcontext from scheduler to next task in queue.
    - A wrapper function wraps the task function and sets the task state to completed when it returns. The scheduler switches to a task right before a task 
      completion check, in case a task finishes before its time slice, it returns to the context scheduler context (task -> ctx.uc_link = &ctx_sched) and the 
      scheduler executes the clean-up code. Otherwise, we preempt and context switch normally.
*/


#endif // SCHEDULER_H