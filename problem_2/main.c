#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include "matrix.h"




int * threadStatus;
pthread_mutex_t accessDataLock;
pthread_mutex_t accessResultsLock;

FILE *filehandle; 
int n_matrices = 0;
int order_matrices = 0;
int mat_index = 0;

int nProcessed = 0;

int count = 0;

//measurements * results;


int getData (int id) {

    // Allocate data for the matrices in question
    size_t data_size = order_matrices * order_matrices;
    double data[data_size];
    

    // This will ensure we will read the exact amount of bytes
    // We are supposed to read


    matrix mat = SQUARE_MATRIX(order_matrices, data);
    mat_index ++;

}

static void *worker (void *par)
{
  unsigned int id = *((unsigned int *) par),                                                          /* consumer id */
               val;                                                                                /* produced value */
  int i;
  
  getData(id);
  //meter lock
  nProcessed ++;                                                                                        /* counting variable */

    
                                                                      


  threadStatus[id] = EXIT_SUCCESS;
  pthread_exit (&threadStatus[id]);
}



void initialize(int N){ //N is nr of Threads

    printf("me");
    
    pthread_t tIdWork[N];                                                        /* workers internal thread id array */
    unsigned int work[N];                                             /* workers application defined thread id array */
    int i;                                                                                        /* counting variable */
    int *status_p;                                                                      /* pointer to execution status */

    /* initializing the application defined thread id arrays for the producers and the consumers and the random number
     generator */

    


    for (i = 0; i < N; i++)
        work[i] = i;
    
    

    /* generation of intervening entities threads */

  
    for (i = 0; i < N; i++)
    { if (pthread_create (&tIdWork[i], NULL, worker, &work[i]) != 0)                                   /* thread producer */
        { perror ("error on waiting for thread producer");
            exit (EXIT_FAILURE);
        }
    }

    

    /* waiting for the termination of the intervening entities threads */

    printf ("\nFinal report\n");
    for (i = 0; i < N; i++)
    { if (pthread_join (tIdWork[i], (void *) &status_p) != 0)                                       /* thread producer */
        { perror ("error on waiting for thread producer");
            exit (EXIT_FAILURE);
        }
        printf ("thread producer, with id %u, has terminated: ", i);
        printf ("its status was %d\n", *status_p);
    }


}


int submitResults(){

}








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

void open_file(const char *filename){
    filehandle = fopen(filename, "r");

    if (filehandle == NULL) {
        printf("Could not open file '%s'. Skipping\n", filename);
        return;
    }
    printf("\n\n\nDeterminants for file '%s'\n\n", filename);

    if (fread(&n_matrices, sizeof(int), 1, filehandle) == -1 ||
        fread(&order_matrices, sizeof(int), 1, filehandle) == -1)
    {
        printf("Unable to read number and size of the matrices. Skipping\n");
    }

    printf("Number of matrices: %d\n", n_matrices);
    printf("Order of the matrices: %d\n", order_matrices);

    

}

static void process_file(const char *filename) {
    
    open_file (filename);

    //error on initialize
    initialize(10);
}


static void print_usage(const char *cmd_name) {
    fprintf(stderr, "\nSynopsis: %s OPTIONS\n", cmd_name);
    fprintf(stderr, "  OPTIONS:\n");
    fprintf(stderr, "  -h        --- print this message\n");
    fprintf(stderr, "  -f        --- the name of the file containing the matrices\n");
}


int main(int argc, char *argv[]) {

    int opt;
    char *filename = NULL;

    while((opt = getopt(argc, argv, ":f:h")) != -1) {

        switch (opt) {
        case 'h': // Help option
            print_usage(basename(argv[0]));
            return EXIT_SUCCESS;
        
        case 'f': // File to process option
            filename = malloc(sizeof(char) * strlen(optarg) + 1); // +1 is for the null terminator character
            if (filename == NULL) {
                fprintf(stderr, "%s: Unable to allocate memory for filename\n", basename(argv[0]));
                return EXIT_FAILURE;
            }
            strcpy(filename, optarg);
            break;
        
        case '?': // Invalid option
            fprintf(stderr, "%s: Invalid option\n", basename(argv[0]));
            print_usage(basename(argv[0]));
            return EXIT_FAILURE;

        case ':': // Missing argument for option
            fprintf(stderr, "%s: File to process was not specified\n", basename(argv[0]));
            print_usage(basename(argv[0]));
            return EXIT_FAILURE;
        }
    }

    if (filename == NULL) {
        fprintf(stderr, "%s: File to process was not specified\n", basename(argv[0]));
        print_usage(basename(argv[0]));
        return EXIT_FAILURE;
    }
    
    process_file(filename);

    // For each of the filenames in argv
    // for (int i = 1; i < argc; i++) {
    //     char *filename = argv[i];
    //     process_file(filename);
    // }
    return 0;
}