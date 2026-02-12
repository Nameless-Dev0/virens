#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stddef.h>
#include "util.h"

typedef struct task task_t;

typedef struct queue {
    task_handle_t* tasks;
    size_t capacity;
    size_t length;
} queue_t;

queue_t* create_queue(task_handle_t* handles, size_t num_of_handles);
void destroy_queue(queue_t** queue);
void enqueue(queue_t* queue, task_t* task);
void dequeue(queue_t* queue);
bool is_empty(const queue_t* queue);
bool is_full(const queue_t* queue);
task_t* top(queue_t* queue);
void advance(queue_t* queue);
void rotate(queue_t* queue);
#endif // QUEUE_H