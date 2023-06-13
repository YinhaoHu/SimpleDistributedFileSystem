#include "sdfs_thread_pool.h"
#include "snftp/sdfs_snftp.h"
#include "event/sdfs_event.h"

#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void* sdfs_threadpool_thread_routine(void* tp_ptr);


void sdfs_threadpool_init(sdfs_threadpool_t* tp_ptr)
{
    tp_ptr->slot_begin = 0;
    tp_ptr->slot_end = 0;
    tp_ptr->slots_count = 0;
    tp_ptr->slots_num = SDFS_THREADPOOL_SLOTS_NUM;
    
    memset(tp_ptr->slots_sockfs_array, 0, sizeof(tp_ptr->slots_sockfs_array));
    pthread_mutex_init(&tp_ptr->mutex, NULL);
    pthread_cond_init(&tp_ptr->cond, NULL);

    for(sdfs_uint_t idx = 0; idx < SDFS_THREADPOOL_THREADS_NUM; ++idx)
    {
        int result;
        
        result = pthread_create(&(tp_ptr->threads_id[idx]), NULL, 
                        sdfs_threadpool_thread_routine, (void*)tp_ptr);
        if(result != 0)
        {
            fprintf(stderr, "threadpool_init: pthread_create\n");
            exit(EXIT_FAILURE);
        }
        pthread_detach(tp_ptr->threads_id[idx]);
    }
}


sdfs_return_code_t sdfs_threadpool_enqueue(sdfs_threadpool_t* tp_ptr, sdfs_sock_fd_t fd)
{
    // If requesters are too many, reject.
    sdfs_return_code_t rc = SDFS_RETURN_CODE_INITIALIZER;
    pthread_mutex_t *mtx = &(tp_ptr->mutex);
    pthread_cond_t *cond = &(tp_ptr->cond);
    sdfs_uint_t newend = (tp_ptr->slot_end+1) % (tp_ptr->slots_num );
 
    pthread_mutex_lock(mtx);
    if( (tp_ptr->slots_count+1) != tp_ptr->slots_num)
    {    
        tp_ptr->slots_count += 1; 
        (tp_ptr->slots_sockfs_array)[tp_ptr->slot_end] = fd;
        (tp_ptr->slot_end) = newend; 
        pthread_cond_signal(cond);
        rc.code = 0;
        rc.status = SDFS_RETURN_CODE_STATUS_SUCCESS;
        rc.text = "ThreadPool enqueue ok.\n";
    }
    else
    {
        rc.code = -1;
        rc.status = SDFS_RETURN_CODE_STATUS_FAILURE;
        rc.text = "ThreadPool rejected one request because it is full\n";
    }
    pthread_mutex_unlock(mtx);

    return rc;
}


sdfs_sock_fd_t sdfs_threadpool_dequeue(sdfs_threadpool_t* tp_ptr)
{
    sdfs_sock_fd_t sockfd = 0;
    pthread_mutex_t *mtx = &(tp_ptr->mutex);
    pthread_cond_t *cond = &(tp_ptr->cond);

    pthread_mutex_lock(mtx); 
    while(tp_ptr->slots_count == 0) 
        pthread_cond_wait(cond, mtx); 
    tp_ptr->slots_count -= 1; 
    sockfd = (tp_ptr->slots_sockfs_array)[tp_ptr->slot_begin];
    tp_ptr->slot_begin = (tp_ptr->slot_begin+1) % (tp_ptr->slots_num);
    pthread_mutex_unlock(mtx);

    return sockfd;
}


static void* sdfs_threadpool_thread_routine(void* tp_ptr)
{
    //After serve, close the socket file descriptor.
    sdfs_threadpool_t* tp = ((sdfs_threadpool_t*)(tp_ptr));
    sdfs_sock_fd_t client_sock_fd = 0; 
    sdfs_snftp_message_t* msg = NULL;
     
    while(1)
    {
        client_sock_fd = sdfs_threadpool_dequeue(tp); 
        while( ( msg = sdfs_event_waiting(client_sock_fd) ) != NULL)
        {  
            sdfs_event_response(client_sock_fd, msg);
            sdfs_snftp_message_free(msg);
        }  
        close(client_sock_fd);
    }

    return NULL;
}