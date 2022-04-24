#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "concurrency.h"
#include "filereader.h"

/**
 * @brief Tells whether all threads were sucessful or not
 * 
 */
static bool success = true;

/**
 * @brief Number of threads to be processed. 
 * 
 */
static size_t n_threads = 0;

/**
 * @brief Exit status of the threads.
 * 
 */
static int *threads_status = NULL;

/**
 * @brief Mutex that controls access to the data read from the files
 * 
 */
static pthread_mutex_t access_data_region = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Mutex that controls access to the results
 * 
 */
static pthread_mutex_t access_results_region = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Number of files for processing
 * 
 */
static size_t n_files = 0;

/**
 * @brief Number of processed files
 * 
 */
static size_t n_files_processed = 0;

/**
 * @brief Name of the files to be processed
 * 
 */
static char **file_names = NULL;

/**
 * @brief Current file being processed
 * 
 */
static circular_buffer_t *cb_file_reader = NULL;

/**
 * @brief Results for the each of the files 
 * 
 */
static measurements *results = NULL;


/**
 * @brief Wrapper functions that serves to lock a mutex and if it
 * fails, prints the error message and exits.
 * 
 * @param thread_id 
 * @param mutex 
 */
static void lock_or_die(int thread_id, pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) != 0) {
        threads_status[thread_id] = errno;
        fprintf(stderr, "Error locking mutex for thread %d: %s\n", thread_id, strerror(errno));
        exit(1);
    }
}

/**
 * @brief Wrapper function that serves to unlock a mutex and if it
 * fails, prints the error message and exits.
 * 
 * @param thread_id 
 * @param mutex 
 */
static void unlock_or_die(int thread_id, pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) != 0) {
        threads_status[thread_id] = errno;
        fprintf(stderr, "Error locking mutex for thread %d: %s\n", thread_id, strerror(errno));
        exit(1);
    }
}

/**
 * @brief Resets the status of all threads to 0.
 * 
 * @param _threads_status The pointer to the statuses of the threads.
 * @param _n_threads Number of threads.
 */
static void reset_threads_status(int *_threads_status, size_t _n_threads) {
    for (size_t i = 0; i < _n_threads; i++) {
        _threads_status[i] = 0;
    }
}

/**
 * @brief Resets the status of the results array.
 * 
 * @param _results Pointer to the results array.
 * @param n_results Number of result structs in the array.
 */
static void reset_results(measurements *_results, size_t n_results) {
    for (size_t i = 0; i < n_results; i++) {
        _results[i].n_words = 0;
        _results[i].n_words_end_cons = 0;
        _results[i].n_words_start_vowel = 0;
    }
}

/**
 * @brief Prints the initialization error and exits
 * 
 */
static void print_error_and_exit() {
    fprintf(stderr, "Error while initializing: %s\n", strerror(errno));
    exit(1);
}

/**
 * @brief Switch to the next file if it's valid otherwise skip and repeat
 * until a valid one is found or all files are processed.
 * 
 */
static void swap_file() {
    circular_buffer_t *new_cb_reader = NULL;

    // While we don't reach the end of file names and we find a valid non empty file
    // keep swaping files.
    n_files_processed++;
    while (n_files_processed < n_files) {
        new_cb_reader = c_b_swap_file(cb_file_reader, file_names[n_files_processed]);

        if (new_cb_reader != NULL && c_b_size(new_cb_reader) != 0) break; 
        n_files_processed++;
    }
}

//
//
// Implementation of the public functions
//
//


void initialize(const size_t _n_files, char **_file_names, const size_t _n_threads) {
    n_threads = _n_threads;
    n_files = _n_files;
    file_names = _file_names;

    if ((threads_status = malloc(sizeof(int) * n_threads)) == NULL) print_error_and_exit();

    reset_threads_status(threads_status, n_threads);

    if ((results = malloc(sizeof(measurements) * n_files)) == NULL) print_error_and_exit();

    reset_results(results, n_files);

    cb_file_reader = c_b_open(file_names[n_files_processed], CHUNK_MAX_SIZE);

    // If the file reader is invalid then swap until a valid one is found
    if (cb_file_reader == NULL) swap_file();
}


bool get_data_portion(
    const int thread_id, int *file_id_out, 
    unsigned char *data_out, size_t *data_size_out
) {
    lock_or_die(thread_id, &access_data_region);

    // If all of the files are processed simply exit
    if (n_files_processed == n_files) {
        unlock_or_die(thread_id, &access_data_region);
        return false;
    }

    // If the buffer isn't full, read everything in it and
    // swap to the next valid file.
    if (c_b_size(cb_file_reader) != c_b_capacity(cb_file_reader)) {

        *data_size_out = c_b_read_all(cb_file_reader, data_out);
        *file_id_out = n_files_processed;
        swap_file();

        unlock_or_die(thread_id, &access_data_region);
        return true;
    }

    // Try read a chunk with at least a minimum size and ending at a space character.
    *data_size_out = c_b_read_chunk_until_delim(cb_file_reader, CHUNK_MIN_SIZE, ' ', data_out);
    *file_id_out = n_files_processed;

    // Fill the reader with more data.
    c_b_fill(cb_file_reader);

    // If the reader is empty, swap to the next valid file
    if (c_b_size(cb_file_reader) == 0) swap_file();

    unlock_or_die(thread_id, &access_data_region);
    return true;
}


void submit_results(const int thread_id, const int file_id, const measurements *data_results) {
    lock_or_die(thread_id, &access_results_region);

    results[file_id].n_words += data_results->n_words;
    results[file_id].n_words_end_cons += data_results->n_words_end_cons;
    results[file_id].n_words_start_vowel += data_results->n_words_start_vowel;

    unlock_or_die(thread_id, &access_results_region);
}


void get_final_results(bool *sucess_out, int **threads_status_out, measurements **results_out) {
    *sucess_out = success;
    *threads_status_out = threads_status;
    *results_out = results;
}

void cleanup() {
    success = true;
    n_threads = 0;
    n_files = 0;
    n_files_processed = 0;
    file_names = NULL;

    if (cb_file_reader != NULL) {
        c_b_close(cb_file_reader);
        cb_file_reader = NULL;
    }

    if (threads_status != NULL) {
        free(threads_status);
        threads_status = NULL;
    }

    if (results != NULL) {
        free(results);
    }
}