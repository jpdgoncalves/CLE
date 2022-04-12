#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "concurrency.h"

/**
 * @brief Number of threads to be processed. 
 * 
 */
static size_t n_threads;

/**
 * @brief Exit status of the threads.
 * 
 */
static int *threads_status;

/**
 * @brief Mutex that controls access to the data read from the files
 * 
 */
static pthread_mutex_t access_data_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Mutex that controls access to the results
 * 
 */
static pthread_mutex_t access_results_Lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Number of files for processing
 * 
 */
static size_t n_files;

/**
 * @brief Number of processed files
 * 
 */
static size_t n_files_processed;

/**
 * @brief Name of the files to be processed
 * 
 */
static char **file_names;

/**
 * @brief List of results for the each of the files 
 * 
 */
static measurements results;


void initialize(size_t _n_files, char **_file_names, size_t _n_threads) {
    n_threads = _n_threads;

    if ((threads_status = malloc(sizeof(int) * _n_threads)) == NULL) {
        fprintf(stderr, "Error while initializing: %s\n", strerror(errno));
        exit(1);
    }

    n_files = _n_files;
    n_files_processed = 0;
    file_names = _file_names;
    results = (measurements) {0, 0, 0};
}


bool get_data_portion(int thread_id, int *file_id_out, char *data_out, size_t *data_size_out) {
    return true;
}


void submit_results(int thread_id, int file_id, measurements *results) {

}


void get_final_results(bool *sucess_out, int *threads_status_out, measurements *results_out) {

}

void cleanup() {
    free(threads_status);
}

//
//
// TEST CODE
//
//

#if 0

int main(int argc, char *argv[]) {

}

#endif