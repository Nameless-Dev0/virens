#ifndef VIRENS_H
#define VIRENS_H
#include <ucontext.h>
#include "util.h"

typedef struct task task_t;
typedef task_t* task_handle_t;

extern task_error_t task_create(task_handle_t* handle, void* (*ufn)(void*), void *uarg, void* uret_val); // optional return, otherwise set to NULL
scheduler_error_t scheduler_init(task_t** list_of_task_handles, size_t num_of_tasks);

// do not call called use macro instead
extern ucontext_t* scheduler_ctx(void);
extern ucontext_t* user_ctx(void);
extern void scheduler_destroy(void);

#define SCHEDULER_RUN() do{                      \
    swapcontext(user_ctx(), scheduler_ctx());    \
    scheduler_destroy();                         \
} while (0);                                     \

#endif // VIRENS_H