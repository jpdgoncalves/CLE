#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "utf8iter.h"

/**
 * @brief Internal function used to calculate the number of bytes
 * that follows the first byte. This function does not check for
 * bogus bytes being passed to it.
 * 
 * @param first_byte 
 * @return size_t 
 */
static size_t count_following_bytes(unsigned char first_byte) {
    unsigned char mask = 0b10000000;
    size_t bit_count = 0;

    while (first_byte & mask) {
        bit_count++;
        first_byte <<= 1;
    }

    //Subtract 1 since 11000000 means a single byte follows.
    return bit_count - 1;
}

#define IS_ASCII(unsigned_value) (unsigned_value) < 0x7f // Determines if it is an ASCII character.
/**
 * @brief Determines if the value is a continuation byte.
 * 
 */
#define IS_CONT_BYTE(unsigned_value) (((unsigned_value) >= 0b10000000) && ((unsigned_value) < 0b11000000))
/**
 * @brief Left shift by x amount of bytes
 * 
 */
#define LEFT_SHIFT_BYTES(unsigned_value, x) ((unsigned_value) << (x * 8))

uint32_t utf8iter_next_char(utf8iter *iter) {
    uint32_t utf8_char = 0x0;
    const unsigned char *line = iter->line;

    // Find the first header byte
    while(!UTF8ITER_REACHED_END(iter)) {

        utf8_char = line[iter->_pointer];
        iter->_pointer++;

        // We found an ASCII/single byte character so we return immediatly.
        //printf("Is ASCII!\n");
        if (IS_ASCII(utf8_char)) return utf8_char;

        // We found the first header byte so we break out of the loop
        //printf("Is Header Byte\n");
        if (!IS_CONT_BYTE(utf8_char)) break;
    }

    //Counter for the number of continuation bytes to expect.
    uint32_t cont_count = count_following_bytes((unsigned char) utf8_char);
    utf8_char = LEFT_SHIFT_BYTES(utf8_char, cont_count);

    //Find either all continuation bytes or an ASCII character.
    while(!UTF8ITER_REACHED_END(iter)) {
        uint32_t next_byte = line[iter->_pointer];
        iter->_pointer++;

        if (IS_ASCII(next_byte)) {
            return next_byte;
        } else if (IS_CONT_BYTE(next_byte)) {
            cont_count--;
            utf8_char |= LEFT_SHIFT_BYTES(next_byte, cont_count);

            if (cont_count == 0) return utf8_char;
        } else {
            cont_count = count_following_bytes((unsigned char) next_byte);
            utf8_char = LEFT_SHIFT_BYTES(next_byte, cont_count);
        }
    }

    return 0x0;
}

//
//
// Under here is test code
//
//

#if 0 //Set to 0 when no longer used in testing.
#include <stdio.h>
void print_u_32_as_bin(uint32_t n) {
  printf("0x");
  for (int32_t i = (sizeof(n) * 8) - 1; i >= 0; i--) {
    uint32_t mask = (((uint32_t) 1) << i);
    if (n & mask)
      printf("1");
    else
      printf("0");
  }
  printf("\n");
}

int main(int argc, char **argv) {
    uint32_t ascii_character = '~';
    uint32_t not_ascii_char = 0xffffffff;
    uint32_t not_ascii_char2 = 0x0000007f;

    printf("is ascii '%s'\n", IS_ASCII(ascii_character) ? "yes" : "no");
    printf("is ascii '%s'\n", IS_ASCII(not_ascii_char) ? "yes" : "no");
    printf("is ascii '%s'\n", IS_ASCII(not_ascii_char2) ? "yes" : "no");

    uint32_t cont_byte = 0b10101010;
    uint32_t also_cont_b = 0b10000001;
    uint32_t not_cont_b = 0b11000000;

    printf("is continuation byte '%s'\n", IS_CONT_BYTE(cont_byte) ? "yes" : "no");
    printf("is continuation byte '%s'\n", IS_CONT_BYTE(also_cont_b) ? "yes" : "no");
    printf("is continuation byte '%s'\n", IS_CONT_BYTE(not_cont_b) ? "yes" : "no");

    uint32_t to_left_shift = 0xff;

    print_u_32_as_bin(to_left_shift);
    print_u_32_as_bin(LEFT_SHIFT_BYTES(to_left_shift, 1));
    print_u_32_as_bin(LEFT_SHIFT_BYTES(to_left_shift, 2));
    print_u_32_as_bin(LEFT_SHIFT_BYTES(to_left_shift, 3));

    char *test_line = "Eu vou á caça!?";
    size_t t_line_size = strlen(test_line);
    utf8iter iter = UTF8ITER(test_line, t_line_size);

    printf("test line size should be 17 and is %lu\n", t_line_size);
    printf("Iterator should not be at the end '%u'\n", UTF8ITER_REACHED_END(&iter));

    while(!UTF8ITER_REACHED_END(&iter)) {
        uint32_t utf8_char = utf8iter_next_char(&iter);
        printf("0x%x\n", utf8_char);
    }
    printf("\n");
}
#endif