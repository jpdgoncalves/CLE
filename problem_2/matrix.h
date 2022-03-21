/**
 * @file matix.h
 * @authors José Gonçalves, Maria João
 * @brief Module containing functions used to operate on a matrix as well the
 * type definition of a matrix.
 * @version 0.1
 * @date 2022-03-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef MATRIX_GUARD
#define MATRIX_GUARD
#include <stdlib.h>

typedef struct matrix
{
    const size_t n_rows;
    const size_t n_columns;
    double *data;
} matrix;

/**
 * @brief Macro to define a square matrix
 * 
 */
#define SQUARE_MATRIX(order, data) {order, order, data};

/**
 * @brief Gets the value at the specified row and column.
 * 
 * @param m
 * @param row 
 * @param column 
 * @return double 
 */
double matrix_get_value(const matrix *m, const size_t row, const size_t column);

/**
 * @brief Set the value at the specified row and column.
 * 
 * @param m 
 * @param row 
 * @param column 
 * @param value 
 * @return double 
 */
void matrix_set_value(matrix *m, const size_t row, const size_t column, const double value);

/**
 * @brief Swaps two rows in a matrix
 * 
 * @param m 
 * @param row_1 
 * @param row_2 
 */
void matrix_swap_rows(matrix *m, const size_t row_1, const size_t row_2);

/**
 * @brief Helper function to apply a transformation need to turn a matrix into
 * a triangular matrix.
 * 
 * @param m
 * @param i
 */
void matrix_apply_transform(matrix *m, const size_t i);

#endif