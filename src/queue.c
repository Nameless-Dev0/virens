#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "task_internal.h"

inline bool is_empty(const queue_t* queue){
    return queue->length == 0;
}

inline bool is_full(const queue_t* queue){
    return queue->capacity == queue->length;
}

void advance(queue_t* queue){
    if(is_empty(queue))
        return;
    dequeue(queue);
}

task_t* top(queue_t* queue){
    assert(queue->length > 0); // getting top of empty queue
    return queue->tasks[0];
}

void rotate(queue_t* queue){
    task_handle_t temp = top(queue);
    dequeue(queue);
    enqueue(queue, temp);
}

queue_t* create_queue(task_handle_t* handles, size_t num_of_handles){
    queue_t* queue = malloc(sizeof(queue_t));
    if(!queue){
        perror("queue allocation failed");
        return NULL;
    }

    queue->tasks = malloc(sizeof(task_handle_t) * num_of_handles);
    if(!queue->tasks){
        perror("queue tasks allocation failed");
        free(queue);
        queue = NULL;
        return NULL;
    }
    memcpy(queue->tasks, handles, num_of_handles * sizeof(task_handle_t));

    queue->capacity = num_of_handles;
    queue->length = 0;
    return queue;
}

void destroy_queue(queue_t** queue){
    if(!(queue) || !((*queue)->tasks)){
        fprintf(stderr, "queue destroy failed, null queue\n");
        return;
    }
    free((*queue)->tasks);
    (*queue)->tasks = NULL;
    free(*queue);
    queue = NULL;
}

void enqueue(queue_t* queue, task_handle_t task){
    if(!(queue) || !(queue->tasks) || !(task)){
        fprintf(stderr, "queue enqueue failed, null queue or task\n");
        return;
    }
    queue->tasks[queue->length++] = task;
}

void dequeue(queue_t* queue){
    if(!(queue) || !(queue->tasks)){
        fprintf(stderr, "queue dequeue failed, null queue\n");
        return;
    }
    if(is_empty(queue)){
        fprintf(stderr, "cannot dequeue from empty queue\n");
        return;
    }

    for (size_t i = 0; i < queue->length - 1; i++){
        queue->tasks[i] = queue->tasks[i + 1];
    }
    queue->length--;
}
