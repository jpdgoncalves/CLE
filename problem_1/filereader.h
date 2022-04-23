/**
 * @file filereader.h
 * @author José Gonçalves, Maria João Sousa
 * @brief This module contains functions that allow to read a file
 * by chunks whose size is controlled by a possible min size, a byte delimiter and
 * the maximum size of the buffer
 * @version 0.1
 * @date 2022-04-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FILEREADER_GUARD
#define FILEREADER_GUARD

#include <stdlib.h>

typedef struct circular_buffer_t {
    unsigned char *buffer;
    FILE *file;
    size_t capacity;
    size_t size;
    size_t write_idx;
    size_t read_idx;
} circular_buffer_t;

size_t c_b_size(circular_buffer_t *circular_buffer);

size_t c_b_capacity(circular_buffer_t *circular_buffer);

size_t c_b_fill(circular_buffer_t *circular_buffer);

size_t c_b_read_all(circular_buffer_t *circular_buffer, unsigned char *out);

size_t c_b_read_chunk_until_delim(
    circular_buffer_t *circular_buffer, size_t min_chunk_size,
    unsigned char delim, unsigned char *out
);

circular_buffer_t *c_b_open(char *filename, size_t buffer_size);

/**
 * @brief This function allows to swap the file that is currently being read
 * by another closing the previous one. This will reset the state of the
 * current buffer and avoid extra memory allocations.
 * 
 * @param circular_buffer The buffer whose file needs to be swaped
 * @param filename The name of the new file
 * @return circular_buffer_t* The reference to the buffer passed as argument if sucessful or NULL if unsucessful.
 */
circular_buffer_t *c_b_swap_file(
    circular_buffer_t *circular_buffer, char *filename
);

void c_b_close(circular_buffer_t *circular_buffer);

#endif