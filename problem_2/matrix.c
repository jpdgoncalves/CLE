#include "matrix.h"
#include <stdlib.h>


double matrix_get_value(const matrix *m, const size_t row, const size_t column) {
    return m->data[m->n_columns * row + column];
}


void matrix_set_value(matrix *m, const size_t row, const size_t column, const double value) {
    m->data[m->n_columns * row + column] = value;
}


void matrix_swap_rows(matrix *m, const size_t row_1, const size_t row_2) {
    const size_t offset_1 = row_1 * m->n_columns;
    const size_t offset_2 = row_2 * m->n_columns;
    const size_t n_columns = m->n_columns;
    double *data = m->data;

    for (size_t column = 0; column < n_columns; column++) {
        const size_t temp = data[offset_1 + column];
        data[offset_1 + column] = data[offset_2 + column];
        data[offset_2 + column] = temp;
    }
}


void matrix_apply_transform(matrix *mat, const size_t i) {
    const size_t n_rows = mat->n_rows;
    const size_t n_columns = mat->n_columns;

    const double cell_ii = matrix_get_value(mat, i, i);

    for (size_t k = i + 1; k < n_rows; k++) {

        const double cell_ki = matrix_get_value(mat, k, i);

        for (size_t j = i; j < n_columns; j++) {
            const double cell_kj = matrix_get_value(mat, k, j);
            const double cell_ij = matrix_get_value(mat, i, j);
            
            matrix_set_value(mat, k, j, cell_kj - (cell_ki / cell_ii * cell_ij));
        }
    }
}

//
//
// Test code. Change to 1 and compile the file to check it is running properly.
//
//
#if 0
#include <stdio.h>

static void print_matrix(matrix *m) {
    const size_t n_rows = m->n_rows;
    const size_t n_columns = m->n_columns;
    const double *data = m->data;

    for (size_t r = 0; r < n_rows; r++) {
        for (size_t c = 0; c < n_columns; c++) {
            printf("%f, ", data[r * n_columns + c]);
        }
        printf("\n");
    }

    printf("\n");
}

void test_get_set_swap() {
    size_t n_rows = 4;
    size_t n_columns = 4;
    double data[16] = {
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    };

    matrix m = SQUARE_MATRIX(n_rows, data);

    printf("Matrix appearance:\n");
    print_matrix(&m);

    printf("Value for cell index 0,1 should be 2 and is %f\n", matrix_get_value(&m, 0, 1));
    printf("Value for cell index 1,2 should be 7 and is %f\n", matrix_get_value(&m, 1, 2));
    printf("Value for cell index 2,3 should be 12 and is %f\n", matrix_get_value(&m, 2, 3));
    printf("Value for cell index 3,3 should be 16 and is %f\n", matrix_get_value(&m, 3, 3));
    printf("Value for cell index 3,0 should be 13 and is %f\n", matrix_get_value(&m, 3, 0));


    printf("Swaping rows 0 and 1\n");
    matrix_swap_rows(&m, 0, 1);
    print_matrix(&m);

    printf("Swaping rows 2 and 3\n");
    matrix_swap_rows(&m, 2, 3);
    print_matrix(&m);

    printf("Swaping rows 0 and 2\n");
    matrix_swap_rows(&m, 0, 2);
    print_matrix(&m);


    printf("Setting values of row 3 to 1\n");
    matrix_set_value(&m, 3, 0, 1.0);
    matrix_set_value(&m, 3, 1, 1.0);
    matrix_set_value(&m, 3, 2, 1.0);
    matrix_set_value(&m, 3, 3, 1.0);
    print_matrix(&m);
}

void test_apply_transform() {
    size_t n_rows = 4;
    size_t n_columns = 4;
    double data[16] = {
         1.0,  2.0,  3.0,  4.0,
         5.0,  11.0,  7.0,  8.0,
         9.0, 10.0, 92.0, 12.0,
        13.0, 14.0, 15.0, 1.0
    };
    matrix m = SQUARE_MATRIX(n_rows, data);

    printf("Original matrix\n");
    print_matrix(&m);

    printf("Applying first transformation\n");
    matrix_apply_transform(&m, 0);
    print_matrix(&m);

    printf("Applying second transformation\n");
    matrix_apply_transform(&m, 1);
    print_matrix(&m);

    printf("Applying third transformation\n");
    matrix_apply_transform(&m, 2);
    print_matrix(&m);
}

int main(int argc, char **argv) {
    test_get_set_swap();
    test_apply_transform();
    return 0;
}

#endif