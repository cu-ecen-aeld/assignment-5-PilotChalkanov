#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // hint: use a cast like the one below to obtain thread arguments from your parameter
    // casting is needed because of the void type func params
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;


    pthread_mutex_t *mutex = thread_func_args->mutex;
    const int wait_to_obtain_ms = thread_func_args->wait_to_obtain_ms;
    const int wait_to_release_ms = thread_func_args->wait_to_release_ms;

    //lock mutex
    usleep(wait_to_obtain_ms*1000);
    if (pthread_mutex_lock(mutex) != 0) {
        ERROR_LOG("mutex lock failed\n");
        thread_func_args->thread_complete_success = false;
        return thread_func_args;
    }

    usleep(wait_to_release_ms*1000);

    // unlock mutex
    if (pthread_mutex_unlock(mutex) != 0) {
        ERROR_LOG("mutex unlock failed\n");
        thread_func_args->thread_complete_success = false;

        return thread_func_args;
    }
    thread_func_args->thread_complete_success = true;
    return thread_func_args;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    struct thread_data* thread_data = malloc(sizeof(struct thread_data));
    if (thread_data == NULL) {
        ERROR_LOG("failed malloc for thread_data");
        return false;
    }

    thread_data->mutex = mutex;
    thread_data->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data->wait_to_release_ms = wait_to_release_ms;
    thread_data->thread_complete_success = false;


    if (pthread_create(thread, NULL, threadfunc, thread_data) != 0) {
        ERROR_LOG("thread creation failed");
        free(thread_data);
        return false;
    }
    return true;

}

