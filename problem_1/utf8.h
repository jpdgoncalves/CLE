/**
 * @file utf8.h
 * @author José Gonçalves
 * @brief Module containing functions that allow to convert utf8 to ascii, determine whether it is a consonant
 * or vowel.
 * @version 0.1
 * @date 2022-03-12
 * 
 */
#ifndef UTF8_GUARD
#define UTF8_GUARD
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Determines whether a utf8 character is alphanumeric or not.
 *  
 * @return true if it is alphanumeric and false otherwise.
 */
bool is_alphanumeric(uint32_t utf8_char);

/**
 * @brief Determines whether a utf8 character is a vowel or not.
 * 
 * @return true if it is a vowel and false otherwise.
 */
bool is_vowel(uint32_t utf8_char);

/**
 * @brief Determines whether a utf8 character is a consonant or not.
 * 
 * @return true if it is a consonant and false otherwise.
 */
bool is_consonant(uint32_t utf8_char);

bool is_separator(uint32_t utf8_char);

bool is_punctuation(uint32_t utf8_char);

bool is_whitespace(uint32_t utf8_char);

bool is_merger(uint32_t utf8_char);

#endif