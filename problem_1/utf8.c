#include "utf8.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

static uint32_t ACCENT_CONV_TABLE[29] = {
    0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,// a
    0x63, // c
    0x65, 0x65, 0x65, 0x65, // e
    0x69, 0x69, 0x69, 0x69, // i
    0x6f, // o
    0x6e, // n
    0x6f, 0x6f, 0x6f, 0x6f, 0x6f, // o
    0x20, 0x20, // whitespace
    0x75, 0x75, 0x75, 0x75 // u
};

static uint32_t conv_latin_to_ascii(uint32_t utf8_char) {
    // 'à' <= utf8_char && utf8_char <= 'ü'
    if (0xc3a0 <= utf8_char && utf8_char <= 0xc3bc) {
        return ACCENT_CONV_TABLE[utf8_char - 0xc3a0];
    }

    // 'À' <= utf8_char && utf8_char <= 'Ü'
    if (0xc380 <= utf8_char && utf8_char <= 0xc39c) {
        return ACCENT_CONV_TABLE[utf8_char + 0x20 - 0xc3a0];
    }
    
    return utf8_char;
}

static bool is_ascii_vowel(uint32_t utf8_char) {
    bool is_lower_vowel = (utf8_char == 'a') || \
                          (utf8_char == 'e') || \
                          (utf8_char == 'i') || \
                          (utf8_char == 'o') || \
                          (utf8_char == 'u');

    bool is_upper_vowel = (utf8_char == 'A') || \
                          (utf8_char == 'E') || \
                          (utf8_char == 'I') || \
                          (utf8_char == 'O') || \
                          (utf8_char == 'U');
    
    return is_lower_vowel || is_upper_vowel;
}

static bool is_ascii_alpha(uint32_t utf8_char) {

    return ('a' <= utf8_char && utf8_char <= 'z') || \
           ('A' <= utf8_char && utf8_char <= 'Z') ;
}

static bool is_ascii_number(uint32_t utf8_char) {

    return ('0' <= utf8_char && utf8_char <= '9');
}

static bool is_ascii_consonant(uint32_t utf8_char) {
    return is_ascii_alpha(utf8_char) && !(is_ascii_vowel(utf8_char));
}

bool is_alphanumeric(uint32_t utf8_char) {
    uint32_t utf8_char_no_acc = conv_latin_to_ascii(utf8_char);

    return  is_ascii_number(utf8_char) || is_ascii_alpha(utf8_char) || \
            is_ascii_alpha(utf8_char_no_acc);
}

bool is_vowel(uint32_t utf8_char) {
    uint32_t utf8_char_no_acc = conv_latin_to_ascii(utf8_char);

    return is_ascii_vowel(utf8_char) ||\
           is_ascii_vowel(utf8_char_no_acc);
}

bool is_consonant(uint32_t utf8_char) {
    uint32_t utf8_char_no_acc = conv_latin_to_ascii(utf8_char);

    return is_ascii_consonant(utf8_char) || \
           is_ascii_consonant(utf8_char_no_acc);
}

bool is_separator(uint32_t utf8_char) {

    return (utf8_char == '-') || (utf8_char == '\"') || (utf8_char == 0xe2809c) || (utf8_char == 0xe2809d) || \
           (utf8_char == '[') || (utf8_char == ']') || (utf8_char == ')') || (utf8_char == '(');
}

bool is_punctuation(uint32_t utf8_char) {
    return (utf8_char == '.') || (utf8_char == ',') || (utf8_char == ':') || (utf8_char == ';') || \
           (utf8_char == '?') || (utf8_char == '!') || \
           (utf8_char == 0xe28093) || (utf8_char == 0xe280a6);
}

bool is_whitespace(uint32_t utf8_char) {
    return (utf8_char == ' ') || (utf8_char == 0x9) || (utf8_char == '\n') || (utf8_char == '\r');
}

bool is_merger(uint32_t utf8_char) {
    return (utf8_char == 0x27) || (utf8_char == 0xe28098) || (utf8_char == 0xe28099);
}

#if 0
#include <stdio.h>
int main(int argc, char **argv) {
    printf("is 0 alphanumeric '%s'\n", is_alphanumeric('0') ? "true" : "false");
    printf("is 9 alphanumeric '%s'\n", is_alphanumeric('9') ? "true" : "false");
    printf("is c alphanumeric '%s'\n", is_alphanumeric('c') ? "true" : "false");
    printf("is a alphanumeric '%s'\n", is_alphanumeric('a') ? "true" : "false");
    printf("is z alphanumeric '%s'\n", is_alphanumeric('z') ? "true" : "false");
    printf("is A alphanumeric '%s'\n", is_alphanumeric('A') ? "true" : "false");
    printf("is Z alphanumeric '%s'\n", is_alphanumeric('Z') ? "true" : "false");
    printf("is à alphanumeric '%s'\n", is_alphanumeric('à') ? "true" : "false");
    printf("is ù alphanumeric '%s'\n", is_alphanumeric('ù') ? "true" : "false");
    printf("is ç alphanumeric '%s'\n", is_alphanumeric('ç') ? "true" : "false");

    printf("is _ alphanumeric '%s'\n", is_alphanumeric('_') ? "true" : "false");
    printf("is ? alphanumeric '%s'\n", is_alphanumeric('?') ? "true" : "false");


    printf("is a vowel '%s'\n", is_vowel('a') ? "true" : "false");
    printf("is á vowel '%s'\n", is_vowel('á') ? "true" : "false");
    printf("is c vowel '%s'\n", is_vowel('c') ? "true" : "false");
    printf("is é vowel '%s'\n", is_vowel('é') ? "true" : "false");
    printf("is í vowel '%s'\n", is_vowel('í') ? "true" : "false");
    printf("is ç vowel '%s'\n", is_vowel('ç') ? "true" : "false");

    printf("is Ç vowel '%s'\n", is_vowel('Ç') ? "true" : "false");
    printf("is Ù vowel '%s'\n", is_vowel('Ù') ? "true" : "false");
    printf("is É vowel '%s'\n", is_vowel('É') ? "true" : "false");
    printf("is T vowel '%s'\n", is_vowel('T') ? "true" : "false");


    printf("is T consonant '%s'\n", is_consonant('T') ? "true" : "false");
    printf("is Ç consonant '%s'\n", is_consonant('Ç') ? "true" : "false");
    printf("is A consonant '%s'\n", is_consonant('A') ? "true" : "false");
    printf("is e consonant '%s'\n", is_consonant('e') ? "true" : "false");
}

#endif