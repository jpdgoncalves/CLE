#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "utf8iter.h"
#include "utf8.h"
#include "concurrency.h"

/**
 * @brief Procedure to calculate the number of words, number of words starting with a vowel
 * and number of words ending with a consonant.
 * 
 * @param data The text to process.
 * @param data_size The size of the text in bytes.
 * @param out Output of the measurements of the text provided.
 */
void process_data(const unsigned char *data, const size_t data_size, measurements *out) {
    uint32_t utf8_char = 0; // The current utf8 character.
    uint32_t prev_utf8_char = 0; // The previous utf8 character.
    utf8iter iter = UTF8ITER(data, data_size); // Iterator of utf8 characters.
    bool in_word = false; // Flag to detected whether we are inside a word or not.
    
    while (!UTF8ITER_REACHED_END(&iter)) {
        prev_utf8_char = utf8_char;
        utf8_char = utf8iter_next_char(&iter);

        if (!in_word) {
            if(is_alphanumeric(utf8_char) || utf8_char == '_') {
                in_word = true;
                out->n_words++;

                if (is_vowel(utf8_char)) {
                    out->n_words_start_vowel++;
                }
            }
        } else {
            if (is_whitespace(utf8_char) || is_punctuation(utf8_char) || is_separator(utf8_char)) {
                in_word = false;

                if (is_consonant(prev_utf8_char)) {
                    out->n_words_end_cons++;
                }
            }
        }
    }
}


/**
 * @brief The procedure to be executed by the worker threads.
 * 
 * @param thread_id_arg An application defined id for the thread.
 * @return void* 
 */
void *thread_procedure(void *thread_id_arg) {
    int thread_id = *((int *) thread_id_arg);
    int file_id;
    size_t data_size;
    unsigned char data[CHUNK_MAX_SIZE];

    while (get_data_portion(thread_id, &file_id, data, &data_size)) {
        measurements results = {0, 0, 0};

        process_data(data, data_size, &results);
        submit_results(thread_id, file_id, &results);
    }

    return 0;
}


void program_usage(char *prog_path) {
    printf("\nUSAGE: .%s -n<number_of_threads> <file_1> [file_n]...\n", strrchr(prog_path, '/'));
    printf("-h\t\tPrints this message\n");
    printf("-n\t\tSets the number of threads\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int number_of_threads = 0;
    char *prog_path = argv[0];

    char *file_names[argc];
    int number_of_files = 0;

    if (argc == 1) {
        printf("No arguments provided\n");
        program_usage(prog_path);
        return 1;
    }

    while ((opt = getopt(argc, argv, "-:n:h")) != -1) {
        switch (opt) {
            case 'h':
                program_usage(prog_path);
                return 0;
            case 'n':
                number_of_threads = atoi(optarg);
                if (number_of_threads < 1) {
                    printf("Option -n must be a number that isn't negative or decimal\n");
                    program_usage(prog_path);
                    return 1;
                }
                break;
            case ':':
                printf("Option -n requires an argument\n");
                program_usage(prog_path);
                return 1;
            case '?':
                printf("Unknown option: %c\n", optopt);
                program_usage(prog_path);
                return 1;
            case 1:
                file_names[number_of_files] = optarg;
                number_of_files++;
                break;
        }
    }

    if (number_of_threads == 0) {
        printf("Number of threads was not specified\n");
        program_usage(prog_path);
        return 1;
    }

    if (number_of_files == 0) {
        printf("No files given for processing\n");
        program_usage(prog_path);
        return 1;
    }

    printf("Number of worker threads: %d\n", number_of_threads);
    printf("Number of files for processing: %d\n", number_of_files);

    //
    // Beginning of the threaded code
    //
    pthread_t threads[number_of_threads];
    int thread_ids[number_of_threads];

    struct timespec start, finish;
    bool threads_success = false;
    int *threads_status;
    measurements *results;

    initialize((size_t) number_of_files, file_names, (size_t) number_of_threads);

    clock_gettime (CLOCK_MONOTONIC_RAW, &start);

    // Create and start the threads.
    for (int thread_idx = 0; thread_idx < number_of_threads; thread_idx++) {
        thread_ids[thread_idx] = thread_idx;
        if (pthread_create(&threads[thread_idx], NULL, thread_procedure, &thread_ids[thread_idx]) == -1) {
            printf("Error creating thread %d: %s\n", thread_idx, strerror(errno));
            return -1;
        }
    }

    // Wait for the threads to finish.
    for (int thread_idx = 0; thread_idx < number_of_threads; thread_idx++) {
        if (pthread_join(threads[thread_idx], NULL) == -1) {
            printf("Error joining thread %d: %s\n", thread_idx, strerror(errno));
            return -1;
        }
    }

    clock_gettime (CLOCK_MONOTONIC_RAW, &finish);

    get_final_results(&threads_success, &threads_status, &results);

    // If there were any thread errors print them and exit with failure status.
    if (!threads_success) {
        for (int thread_idx = 0; thread_idx < number_of_threads; thread_idx++) {

            if (threads_status[thread_idx] != 0) 
                printf("Error at thread %d: %s\n", thread_idx, strerror(threads_status[thread_idx]));
        }

        return 1;
    }

    // Everything went smoothly. We can print the results
    for (int file_idx = 0; file_idx < number_of_files; file_idx++) {
        char *file_name = file_names[file_idx];
        measurements result = results[file_idx];

        printf("\nFile name: %s\n", file_name);
        printf("Number of words = %lu\n", result.n_words);
        printf("Number of words that start with vowel = %lu\n", result.n_words_start_vowel);
        printf("Number of words that start with consonant = %lu\n", result.n_words_end_cons);
    }

    cleanup(); 

    printf ("\nElapsed time = %.6f s\n",  (finish.tv_sec - start.tv_sec) / 1.0 + (finish.tv_nsec - start.tv_nsec) / 1000000000.0);

    return 0;
}