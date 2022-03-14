#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utf8iter.h"
#include "utf8.h"

typedef struct measurements {
    size_t n_words;
    size_t n_words_start_vowel;
    size_t n_words_end_cons;
} measurements;

void process_line(const char *data, const size_t data_size, measurements *out) {
    uint32_t utf8_char = 0; // The current utf8 character.
    uint32_t prev_utf8_char = 0; // The previous utf8 character.
    utf8iter iter = UTF8ITER(data, data_size); // Iterator of utf8 characters.
    bool in_word = false; // Flag to detected whether we are inside a word or not.
    
    while (!UTF8ITER_REACHED_END(&iter)) {
        prev_utf8_char = utf8_char;
        utf8_char = utf8iter_next_char(&iter);

        if (!in_word) {
            if(is_alphanumeric(utf8_char)) {
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


int main(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        const char *filename = argv[i];
        FILE *f = fopen(filename, "r");

        if (f == NULL) {
            printf("Couldn't open file '%s'. Skipping\n", filename);
            continue;
        }

        // Loop through the file one line at a time
        // And for each line determine the number of words,
        // the number of words the start with a vowel
        // and the number of words that end with a consonant.
        char *line_buffer = NULL;
        size_t buffer_size = 0;
        ssize_t line_size = 0;
        measurements m = {0, 0, 0};
        while( (line_size = getline(&line_buffer, &buffer_size, f)) != -1) {
            process_line(line_buffer, (size_t) line_size, &m);
        }

        printf("Measurements for %s\n", filename);
        printf("Number of words %lu\n", m.n_words);
        printf("Number of words that start with vowel %lu\n", m.n_words_start_vowel);
        printf("Number of words that start with consonant %lu\n", m.n_words_end_cons);

        free(line_buffer);
        fclose(f);
    }

    // const char *str = "Por favor, eu quero ir para casa hoje! Tenho de ir á caça!!";
    // size_t size = strlen(str);
    // measurements m = {0, 0, 0};
    
    // process_line(str, size, &m);

    // printf("Number of words %lu\n", m.n_words);
    // printf("Number of words that start with vowel %lu\n", m.n_words_start_vowel);
    // printf("Number of words that start with consonant %lu\n", m.n_words_end_cons);

    return 0;
}