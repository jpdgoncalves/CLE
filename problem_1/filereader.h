/**
 * @file filereader.h
 * @author José Gonçalves, Maria João Sousa
 * @brief This module contains functions that allow to read a file
 * by chunks whose size is controlled by a possible min size, a byte delimiter and
 * the maximum size of the buffer
 * @version 0.1
 * @date 2022-04-23
 * 
 */

#ifndef FILEREADER_GUARD
#define FILEREADER_GUARD

#include <stdlib.h>

/**
 * @brief Data structure representing a circular buffer
 * It's fields must not be changed directly.
 * 
 */
typedef struct circular_buffer_t {
    unsigned char *buffer;
    FILE *file;
    size_t capacity;
    size_t size;
    size_t write_idx;
    size_t read_idx;
} circular_buffer_t;

/**
 * @brief Get the amount of bytes stored in the data buffer
 * 
 * @param circular_buffer 
 * @return size_t The amount of bytes in the buffer.
 */
size_t c_b_size(circular_buffer_t *circular_buffer);

/**
 * @brief Get the maximum amount of bytes the buffer can hold
 * 
 * @param circular_buffer 
 * @return size_t The amount of bytes the buffer can hold.
 */
size_t c_b_capacity(circular_buffer_t *circular_buffer);

/**
 * @brief Attempts to fill the buffer with more data from the file,
 * if there is any and if there is any free space to store it.
 * 
 * @param circular_buffer 
 * @return size_t The amount of bytes written to the buffer.
 */
size_t c_b_fill(circular_buffer_t *circular_buffer);

/**
 * @brief Reads all of the data in the buffer into the output buffer.
 * It is assumed that enough data was allocated for the output buffer.
 * 
 * @param circular_buffer 
 * @param out The output buffer for the data
 * @return size_t The amount of bytes read into the output buffer.
 */
size_t c_b_read_all(circular_buffer_t *circular_buffer, unsigned char *out);

size_t c_b_read_chunk_until_delim(
    circular_buffer_t *circular_buffer, size_t min_chunk_size,
    unsigned char delim, unsigned char *out
);

/**
 * @brief Creates a new circular buffer for the specified file. At the start
 * it will try to fill buffer with as many bytes as specified by buffer_size.
 * 
 * If the file could not be open or memory could not be allocated, allocated memory
 * will be free and the opened file closed.
 * 
 * @param filename The name of the file
 * @param buffer_size The size of the buffer
 * @return circular_buffer_t* A pointer to the circular buffer on success or 
 * NULL on failure.
 */
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

/**
 * @brief Closes the buffer by closing the file
 * and deallocating the memory for the buffer struct.
 * 
 * @param circular_buffer 
 */
void c_b_close(circular_buffer_t *circular_buffer);

#endif