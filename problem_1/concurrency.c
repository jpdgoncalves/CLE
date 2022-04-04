#include <stdlib.h>
#include <pthread.h>
#include "concurrency.h"

/**
 * @brief Exit status of the threads.
 * 
 */
int *threads_status;

/**
 * @brief Mutex that controls access to the data read from the files
 * 
 */
static pthread_mutex_t access_data_lock;

/**
 * @brief Mutex that controls access to the results
 * 
 */
static pthread_mutex_t access_results_Lock;

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
static char **fileNames;

/**
 * @brief List of results for the each of the files 
 * 
 */
static measurements *results;


void initialize(size_t n_files, char **file_names) {

}


bool get_data_portion(int thread_id, int *file_id_out, char *data_out, size_t *data_size_out) {

}


void submit_results(int thread_id, int file_id, measurements *results) {

}


void get_final_results(bool *sucess_out, int *threads_status_out, measurements **results_out) {
    
}