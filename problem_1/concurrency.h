/**
 * @file concurrency.h
 * @authors José Gonçalves, Maria João Sousa
 * @brief Module containing the access primitives to the shared region as well as
 * a typedef used to store partial results.
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <stdbool.h>


/**
 * @brief Struct definition used to store results for a portion of data from a file
 * or the results of the file as a whole.
 * 
 */
typedef struct measurements {
    size_t n_words;
    size_t n_words_start_vowel;
    size_t n_words_end_cons;
} measurements;

/**
 * @brief Expected minimum size that will be read into the data output buffer
 * for get_data_portion()
 * 
 */
#define CHUNK_MIN_SIZE 512

/**
 * @brief Maximum amount of data read into the the data output buffer for
 * get_data_portion()
 * 
 */
#define CHUNK_MAX_SIZE CHUNK_MIN_SIZE * 2

/**
 * @brief Function used to initialize the shared region variables.
 * 
 * @param n_files 
 * @param file_names
 * @param n_threads 
 */
void initialize(const size_t n_files, char **file_names, const size_t n_threads);


/**
 * @brief Get a portion of data for processing. If the function
 * returns false it means there wasn't anymore data to process and
 * the thread should quit.
 * 
 * @param thread_id The id of the thread.
 * @param file_id_out Id of the file the portion of data belongs to.
 * @param data_out Pointer to the buffer where the data will be stored.
 * @param data_size_out The amount of bytes copied into the buffer.
 * @return true if the thread should continue or false if it should exit.
 */
bool get_data_portion(
    const int thread_id, int *file_id_out, 
    unsigned char *data_out, size_t *data_size_out
);


/**
 * @brief Submit the results obtained after processing a portion data
 * of some file.
 * 
 * @param thread_id The id of the thread.
 * @param file_id The id of the file the processed data belonged to.
 * @param results The results obtained from processing.
 */
void submit_results(const int thread_id, const int file_id, const measurements *results);


/**
 * @brief Get results from the processing including whether there were any errors and on which threads
 * 
 * @param sucess_out If no threads had errors during their execution.
 * @param threads_status_out The statuses of the threads. This is used to know which thread failed and why.
 * @param results_out The measurements made for each of the files.
 */
void get_final_results(bool *sucess_out, int **threads_status_out, measurements **results_out);

/**
 * @brief Cleans up the memory region after being used. This function should only be called
 * after all threads accessing it have exited
 * 
 */
void cleanup();