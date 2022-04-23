#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "filereader.h"

static void unsafe_write_byte(circular_buffer_t *circular_buffer, unsigned char byte) {
    size_t capacity = circular_buffer->capacity;
    size_t write_idx = circular_buffer->write_idx;

    circular_buffer->buffer[write_idx] = byte;
    circular_buffer->write_idx = (write_idx + 1) % capacity;
    circular_buffer->size++;
}


static void unsafe_read_byte(circular_buffer_t *circular_buffer, unsigned char *byte_out) {
    size_t read_idx = circular_buffer->read_idx;
    
    *byte_out = circular_buffer->buffer[read_idx];
    circular_buffer->read_idx = (read_idx + 1) % circular_buffer->capacity;
    circular_buffer->size--;
}


// static bool write_single_byte(circular_buffer_t *circular_buffer, unsigned char byte) {
//     if (circular_buffer->size == circular_buffer->capacity) return false;

//     unsafe_write_byte(circular_buffer, byte);

//     return true;
// }


static bool read_single_byte(circular_buffer_t *circular_buffer, unsigned char *byte_out) {
    if (circular_buffer->size == 0) return false;

    unsafe_read_byte(circular_buffer, byte_out);

    return true;
}


static size_t free_bytes(circular_buffer_t *circular_buffer) {
    return circular_buffer->capacity - circular_buffer->size;
}

//
//
// PUBLIC FUNCTIONS
//
//


size_t c_b_size(circular_buffer_t *circular_buffer) {
    return circular_buffer->size;
}


size_t c_b_capacity(circular_buffer_t *circular_buffer) {
    return circular_buffer->capacity;
}


size_t c_b_fill(circular_buffer_t *circular_buffer) {
    unsigned char temp_buffer[circular_buffer->capacity];
    size_t bytes_read = fread(temp_buffer, 1, free_bytes(circular_buffer), circular_buffer->file);

    for (size_t tb_read_idx = 0; tb_read_idx < bytes_read; tb_read_idx++) {
        unsafe_write_byte(circular_buffer, temp_buffer[tb_read_idx]);
    }

    return bytes_read;
}

size_t c_b_read_all(circular_buffer_t *circular_buffer, unsigned char *out) {
    size_t buffer_size = circular_buffer->size;

    while (read_single_byte(circular_buffer, out)) {
        out++;
    }

    return buffer_size;
}


size_t c_b_read_chunk_until_delim(
    circular_buffer_t *circular_buffer, size_t min_chunk_size,
    unsigned char delim, unsigned char *out
) {
    size_t read_bytes = 0;

    while (read_single_byte(circular_buffer, out)) {
        read_bytes++;
        if (read_bytes >= min_chunk_size && out[0] == delim) break;
        out++;
    }
    
    return read_bytes;
}


circular_buffer_t *c_b_open(char *filename, size_t buffer_size) {
    unsigned char *buffer;
    FILE *file;
    circular_buffer_t *circular_buffer;

    if ((buffer = malloc(buffer_size)) == NULL) {
        printf("Error alocating memory for the buffer: %s\n", strerror(errno));
        return NULL;
    }

    if ((file = fopen(filename, "r")) == NULL) {
        printf("Error opening the file: %s\n", strerror(errno));
        free(buffer);
        return NULL;
    }

    if ((circular_buffer = malloc(sizeof(circular_buffer_t))) == NULL) {
        printf("Error allocating memory for the struct: %s\n", strerror(errno));
        free(buffer);
        fclose(file);
        return NULL;
    }

    circular_buffer->buffer = buffer;
    circular_buffer->file = file;
    circular_buffer->capacity = buffer_size;
    circular_buffer->size = 0;
    circular_buffer->write_idx = 0;
    circular_buffer->read_idx = 0;

    c_b_fill(circular_buffer);

    return circular_buffer;
}


circular_buffer_t *c_b_swap_file(
    circular_buffer_t *circular_buffer, char *filename
) {

    FILE *new_file;

    if ((new_file == fopen(filename, "r")) == NULL) {
        printf("Unable to open new file: %s\n", strerror(errno));
        return NULL;
    }

    fclose(circular_buffer->file);

    circular_buffer->file = new_file;
    circular_buffer->size = 0;
    circular_buffer->write_idx = 0;
    circular_buffer->read_idx = 0;

    c_b_fill(circular_buffer);

    return circular_buffer;
}


void c_b_close(circular_buffer_t *circular_buffer) {
    fclose(circular_buffer->file);
    free(circular_buffer->buffer);
    free(circular_buffer);
}

//
//
// TEST CODE
//
//

// void print_char_arr(unsigned char *arr, size_t size) {
//     for (size_t i = 0; i < size; i++) {
//         putchar(arr[i]);
//     }
// }

// int main() {
//     // circular_buffer_t *circular_buffer = circular_buffer_open("./data/text2.txt", 10);
//     // printf("buffer size: %lu\n", circular_buffer->capacity);
//     // printf("free bytes: %lu\n", free_bytes(circular_buffer));

//     // while (write_single_byte(circular_buffer, 'A')) {
//     //     printf("Written the byte 'A' to the buffer\n");
//     // }

//     // unsigned char byte_out;
//     // while (read_single_byte(circular_buffer, &byte_out)) {
//     //     printf("Read byte ");
//     //     putchar(byte_out);
//     //     putchar('\n');
//     // }

//     // while (write_single_byte(circular_buffer, 'B')) {
//     //     printf("Written byte 'B'\n");
//     // }

//     // while (read_single_byte(circular_buffer, &byte_out)) {
//     //     printf("Read byte ");
//     //     putchar(byte_out);
//     //     putchar('\n');
//     // }


//     // printf("\nTesting the fill_buffer function\n");
//     // while (fill_buffer(circular_buffer)) {
//     //     while(read_single_byte(circular_buffer, &byte_out)) {
//     //         putchar(byte_out);
//     //     }
//     // }

//     // while(read_single_byte(circular_buffer, &byte_out)) {
//     //     putchar(byte_out);
//     // }

//     // circular_buffer_close(circular_buffer);

//     // printf("\n Testing the read_all_buffer function\n");

//     // circular_buffer = circular_buffer_open("./data/text1.txt", 20);
//     // unsigned char data_out[20];

//     // while(fill_buffer(circular_buffer)) {
//     //     size_t read_bytes = read_all_buffer(circular_buffer, data_out);

//     //     for (size_t i = 0; i < read_bytes; i++) {
//     //         putchar(data_out[i]);
//     //     }
//     // }

//     // size_t read_bytes = read_all_buffer(circular_buffer, data_out);

//     // for (size_t i = 0; i < read_bytes; i++) {
//     //     putchar(data_out[i]);
//     // }

//     // circular_buffer_close(circular_buffer);

//     printf("\n\nTesting use case for the planed algorithm\n");

//     circular_buffer_t *circular_buffer = circular_buffer_open("./data/text3.txt", 1024);
//     unsigned char data_out[1024];

//     while (circular_buffer_size(circular_buffer) != 0) {
//         size_t read_bytes = 0;

//         if (circular_buffer_size(circular_buffer) != circular_buffer_capacity(circular_buffer)) {
//             read_bytes = read_all_buffer(circular_buffer, data_out);
//             // printf("Read final chunk of %lu bytes.\n", read_bytes);
//             print_char_arr(data_out, read_bytes);
//             break;
//         }

//         read_bytes = read_chunk_until_delim(circular_buffer, 512, ' ', data_out);
//         //printf("Read chunk of %lu bytes.\n", read_bytes);
//         print_char_arr(data_out, read_bytes);

//         fill_buffer(circular_buffer);
//         //printf("Written %lu bytes back to buffer.\n", fill_buffer(circular_buffer));
//     }

//     circular_buffer_close(circular_buffer);
//     return 0;
// }