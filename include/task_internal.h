#ifndef TASK_INTERNAL_H
#define TASK_INTERNAL_H

#include <stdint.h>
#include <ucontext.h>
#include "util.h"

#define STACK_SIZE 256 * 1024

typedef enum{
    READY,
    RUNNING,
    COMPLETED
} task_state_t;

typedef struct task{
    uint32_t ID;
    ucontext_t ctx;
    task_state_t state;
    void* (*fn)(void*);
    void* arg;
    void* ret_val;
} task_t;

task_error_t task_create(task_handle_t* handle, void* (*ufn)(void*), void *uarg, void* uret_val); // optional return, otherwise set to NULL
task_error_t task_destroy(task_handle_t handle);

#endif // TASK_INTERNAL_H