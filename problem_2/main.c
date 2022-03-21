#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "matrix.h"

/**
 * @brief Tries to find a row below the specified row whose cell value at 
 * the specified index is non zero and swaps them with the specified row.
 * 
 * @param mat The matrix where the row is at.
 * @param index The index of the row and the value.
 * @return true if it succeeds and false if it doesn't.
 */
static bool try_swap_row_with_non_zero(matrix *mat, const size_t index) {
    const size_t n_rows = mat->n_rows;
    for (size_t row = index + 1; row < n_rows; row++) {
        if (matrix_get_value(mat, row, index)) {
            matrix_swap_rows(mat, row, index);
            return true;
        }
    }

    return false;
}

static double calculate_det_triang_mat(matrix *m) {
    const size_t n_rows = m->n_rows;
    double determinant = 1;

    for (size_t i = 0; i < n_rows; i++) {
        determinant *= matrix_get_value(m, i, i);
    }

    return determinant;
}

/**
 * @brief Calculates the determinant of a matrix.
 * This function performs changes on the matrix and as such
 * it becomes unusable afterwards.
 * 
 * @param mat 
 * @return double 
 */
static double calculate_determinant(matrix *mat) {
    const size_t n_rows = mat->n_rows;
    double signal = 1;

    // This loop will transform any
    // square matrix into a triangular matrix.
    for (size_t i = 0; i < n_rows - 1; i++) {
        if (matrix_get_value(mat, i, i) == 0) {
            if (try_swap_row_with_non_zero(mat, i)) {
                signal = -1 * signal;
            } else {
                return 0;
            }
        }
        // Subtract from all the rows below i, the ith row
        // In such a way that the ith element of all the rows below i
        // is 0.
        matrix_apply_transform(mat, i);
    }

    // Apply determinant calculation for triangular matrices.
    return signal * calculate_det_triang_mat(mat);
}


int main(int argc, char **argv) {

    // For each of the filenames in argv
    for (int i = 1; i < argc; i++) {

        const char *filename = argv[i];
        FILE *filehandle = fopen(filename, "r");

        if (filehandle == NULL) {
            printf("Could not open file '%s'. Skipping\n", filename);
            continue;
        }        

        printf("\n\n\nDeterminants for file '%s'\n\n", filename);

        int n_matrices = 0;
        int order_matrices = 0;

        if (fread(&n_matrices, sizeof(int), 1, filehandle) == -1 ||
            fread(&order_matrices, sizeof(int), 1, filehandle) == -1
        ) {
            printf("Unable to read number and size of the matrices. Skipping\n");
        }

        printf("Number of matrices: %d\n", n_matrices);
        printf("Order of the matrices: %d\n", order_matrices);

        // Allocate data for the matrices in question
        size_t data_size = order_matrices * order_matrices;
        double data[data_size];
        int mat_index = 0;

        //This will ensure we will read the exact amount of bytes
        //We are supposed to read
        while(fread(data, sizeof(double), data_size, filehandle) == data_size) {
            matrix mat = SQUARE_MATRIX(order_matrices, data);
            double determinant = calculate_determinant(&mat);

            mat_index++;
            printf("Determinant for matrix %d is %e.\n", mat_index, determinant);
        }
    }

    return 0;
}