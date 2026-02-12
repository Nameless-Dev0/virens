#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <error.h>
#include "task_internal.h"
#include "scheduler.h"

static inline uint32_t ID_allocator(void){ // monotonic counter uniquely identifies up to 2^32 numbers
    static uint32_t count = 0;
    return count++;
}

static void task_function(uintptr_t arg){
    task_handle_t handle = (task_handle_t)(arg);
    if(handle->ret_val == NULL)
        handle->fn(handle->arg);
    else
        handle->ret_val = handle->fn(handle->arg);

    handle->state = COMPLETED;
}

task_error_t task_create(task_handle_t* handle, void* (*ufn)(void*), void *uarg, void* uret_val){
    *handle = malloc(sizeof(task_t));
    if(!(*handle)){
        perror("Task allocation failed");
        return TASK_ALLOCATION_FAIL;
    }

    if(getcontext(&(*handle)->ctx) == -1){
        perror("context save failed");
        free(*handle);
        return CONTEXT_SAVE_FAILED;
    }

    (*handle)->ctx.uc_stack.ss_sp = malloc(STACK_SIZE);
    if(!(*handle)->ctx.uc_stack.ss_sp){
        perror("Task stack allocation failed");
        free(*handle);
        return STACK_ALLOCATION_FAIL;
    }
    (*handle)->ctx.uc_stack.ss_size = STACK_SIZE;
    (*handle)->ctx.uc_stack.ss_flags = 0;
    (*handle)->ctx.uc_link = scheduler_ctx();  // return to scheduler

    makecontext(&(*handle)->ctx, (void(*)())task_function, 1, (uintptr_t)(*handle));


    (*handle)->fn = ufn;
    (*handle)->arg = uarg;
    (*handle)->ret_val = uret_val;
    (*handle)->ID = ID_allocator();
    (*handle)->state  = READY;
    return T_SUCCESS;
}


task_error_t task_destroy(task_handle_t handle){ // Called by the scheduler only (task cannot deallocate its own stack)
    if(!handle){
        fprintf(stderr, "task clean up failed: null handle\n");
        return TASK_CLEAN_UP_FAIL;
    }
    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);

    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    free(handle->ctx.uc_stack.ss_sp);
    handle->ctx.uc_stack.ss_sp = NULL;
    handle->ctx.uc_stack.ss_size = 0;
    free(handle);
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    return T_SUCCESS;
}