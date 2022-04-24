#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "matrix.h"


//structure needed to send all important information to the workers
struct info {
    int prod;
    int order_matrices;
    FILE *filehandle;
    int *statusProd;
    int n_matrices;
};

//lock access to read a matrix
pthread_mutex_t getMatrix = PTHREAD_MUTEX_INITIALIZER;

//lock access to add time
static pthread_mutex_t addTime = PTHREAD_MUTEX_INITIALIZER;

//elapsed time
double elapsedTime;

//results
double *result;

//number of matrices that were already processed
int nr_matrices_processed = 0;

//indicates whether or not there are still matrices to process
bool stillProcessing = true;



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



/**
 * @brief Life cycle of the thread worker.
 * Gets data and sends it to be processed by calculateMatrix.
 * 
 * @param filehandle File with the data
 * @param order_matrices 
 * @param n_matrices  
 */
static void life_cycle (FILE *filehandle, int order_matrices, int n_matrices){

    size_t data_size = order_matrices * order_matrices;
    double data[data_size];
    int index = -1;

    //sets timer 
    struct timespec start, finish;   
         
        
    //lock
    pthread_mutex_lock(&getMatrix);

    //checks if it can stiil read data
    if (stillProcessing == true){

        clock_gettime (CLOCK_MONOTONIC_RAW, &start); 

        //read data
        if (fread(data, sizeof(double), data_size, filehandle) == -1)
        {
            printf("Unable to read data. Skipping\n");
        }
        index = nr_matrices_processed;

        nr_matrices_processed ++;
    } 

    if (nr_matrices_processed >= n_matrices){
         stillProcessing = false;
    }

    //unlock
    pthread_mutex_unlock(&getMatrix);

    //if there is data to process
    if (index != -1){

        

        //creates a matrix and calculates its determinant
        matrix mat = SQUARE_MATRIX(order_matrices, data);
        double determinant = calculate_determinant(&mat);

        clock_gettime (CLOCK_MONOTONIC_RAW, &finish);

        //add processing time to shared variable

        //lock
        pthread_mutex_lock(&addTime);

        //add time
        elapsedTime +=  (finish.tv_sec - start.tv_sec) / 1.0 + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        //unlock
        pthread_mutex_unlock(&addTime);

        result [index] = determinant;
       
    }
   
}


/**
 * @brief Thread worker.
 * Receives data, which is an info structure with all of the data necessary for the execution.
 * 
 * @param data structure of the type info
 */
static void *worker (void *data)
{
    struct info *info = data;

    int id = info->prod;  /* worker id */

    //life cycle of the thread
    while(stillProcessing == true){

        life_cycle(info->filehandle, info->order_matrices, info->n_matrices); 

    }     

    info->statusProd[id] = EXIT_SUCCESS;
    pthread_exit (&info->statusProd[id]);
        
}


/**
 * @brief Instantiates shared resources, creates and starts the worker threads, waits for them to finish the work and prints the results.
 * 
 * @param filename Name of the file to process
 * @param number_of_threads Number of threads that will be initiated to handle the task
 */
static void process_file(const char *filename, int number_of_threads) {

    //initiate shared timer
    elapsedTime = 0.0;

    //read file
    FILE *filehandle = fopen(filename, "r");
    if (filehandle == NULL) {
        printf("Could not open file '%s'. Skipping\n", filename);
        return;
    }

    printf("\n\n\nDeterminants for file '%s'\n\n", filename);


    //get number of matrices and their order
    int n_matrices = 0;
    int order_matrices = 0;

    if (fread(&n_matrices, sizeof(int), 1, filehandle) == -1 ||
        fread(&order_matrices, sizeof(int), 1, filehandle) == -1)
    {
        printf("Unable to read number and size of the matrices. Skipping\n");
    }

    printf("Number of matrices: %d\n", n_matrices);
    printf("Order of the matrices: %d\n", order_matrices);


    /* generation of intervening entities threads */
    int N = number_of_threads; //number of threads

    printf("Number of threads = %d \n\n", N);
    //allocate memory for results
    result = malloc(sizeof(double) * n_matrices);

    pthread_t tIdProd[N];
    int statusProd[N];

    for (int i = 0; i < N; i++){
   
        struct info *info = malloc(sizeof(struct info));

        info->prod = i;
        info->order_matrices = order_matrices;
        info->filehandle = filehandle;
        info->statusProd = statusProd;        
        info->n_matrices = n_matrices;


        if (pthread_create (&tIdProd[i], NULL, worker, info) != 0)                              /* thread worker */
        { perror ("error on creating thread worker");
            exit (EXIT_FAILURE);
        } 
    }

    //wait for all threads to finish execution
    for (int i = 0; i < N; i++)
    { if (pthread_join (tIdProd[i], (void *) &statusProd) != 0)                                       /* thread worker */
        { perror ("error on waiting for thread worker");
            exit (EXIT_FAILURE);
        }        
    }

    //print results and overall processing time
    printf ("\nFinal report\n");
    for (int i = 0; i < n_matrices; i++){
        printf("Determinant for matrix %d is %11.3e.\n", i + 1 , result[i]);
    }
    fclose(filehandle);

    printf ("\nElapsed time = %.6f s\n", elapsedTime);
}


static void print_usage(const char *cmd_name) {
    fprintf(stderr, "\nSynopsis: %s OPTIONS\n", cmd_name);
    fprintf(stderr, "  OPTIONS:\n");
    fprintf(stderr, "  -h        --- print this message\n");
    fprintf(stderr, "  -f        --- the name of the file containing the matrices\n");
    fprintf(stderr, "  -n        --- number of threads that will be processing. Default = 10\n");
}


int main(int argc, char *argv[]) {

    int opt;
    char *filename = NULL;
    int number_of_threads = 10;

    while((opt = getopt(argc, argv, ":f:n:h")) != -1) {

        switch (opt) {
        case 'h': // Help option
            print_usage(basename(argv[0]));
            return EXIT_SUCCESS;

        case 'n':
            number_of_threads = atoi(optarg);
            if (number_of_threads < 1) {
                printf("Option -n must be a number that isn't negative or decimal\n");
                //program_usage(prog_path);
                return 1;
            }
            break;
        
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
    
    process_file(filename, number_of_threads);

    return 0;
}