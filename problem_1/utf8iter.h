/**
 * @file lineiter.h
 * @author José Gonçalves
 * @brief A small module used to process utf-8 characters one by one
 * @version 0.1
 * @date 2022-03-11
 * 
 * 
 */

#ifndef LINE_ITER_GUARD
#define LINE_ITER_GUARD

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct utf8iter {
    const unsigned char *line;
    size_t size;
    size_t _pointer;
} utf8iter;

#define UTF8ITER(line, line_size) {line, line_size, 0};
#define UTF8ITER_REACHED_END(iter) ((iter)->_pointer >= (iter)->size) // Determine if we reached the end of the iterator.

/**
 * @brief Gets the next valid utf-8 character.
 * This function will skip over invalid character sequences.
 * 
 * @param iter The line iterator
 * @return A valid utf-8 character or 0x0 if it has reached the end.
 */
uint32_t utf8iter_next_char(utf8iter *iter);

#endif