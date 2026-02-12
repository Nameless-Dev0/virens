#ifndef UTIL_H
#define UTIL_H

/* A collection of types used by both public user API and internal implementations */

#ifndef TASK_HANDLE
#define TASK_HANDLE
    typedef struct task task_t;
    typedef task_t* task_handle_t;
#endif // TASK_HANDLE

typedef enum{
    T_SUCCESS,
    INVALID_TASK_HANDLE,
    STACK_ALLOCATION_FAIL,
    TASK_ALLOCATION_FAIL,
    CONTEXT_SAVE_FAILED,
    CONTEXT_CREATION_FAILED,
    TASK_CLEAN_UP_FAIL
} task_error_t;

typedef enum {
    S_SUCCESS,
    SCHEDULER_READY_Q_ALLOCATION_FAIL,
    SCHEDULER_STACK_ALLOCATION_FAIL,
    SCHEDULER_CONTEXT_SAVE_FAILED,
    SCHEDULER_CLEAN_UP_FAIL
} scheduler_error_t;

#endif // UTIL_H