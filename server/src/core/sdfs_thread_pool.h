#ifndef _SDFS_THREADPOOL_H_
#define _SDFS_THREADPOOL_H_

#define SDFS_THREADPOOL_SLOTS_NUM (64)
#define SDFS_THREADPOOL_THREADS_NUM (16)

#include "sdfs_core.h"
#include <pthread.h>

typedef struct _sdfs_threadpool{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t threads_id[SDFS_THREADPOOL_THREADS_NUM];

    sdfs_uint_t slots_num;
    sdfs_uint_t slots_count;
    sdfs_uint_t slot_begin; 
    sdfs_uint_t slot_end; 
    
    sdfs_sock_fd_t slots_sockfs_array[SDFS_THREADPOOL_SLOTS_NUM]; 
}sdfs_threadpool_t;
 

void sdfs_threadpool_init(sdfs_threadpool_t* tp_ptr);
sdfs_return_code_t sdfs_threadpool_enqueue(sdfs_threadpool_t* tp_ptr, sdfs_sock_fd_t fd);
sdfs_sock_fd_t sdfs_threadpool_dequeue(sdfs_threadpool_t* tp_ptr);


#endif