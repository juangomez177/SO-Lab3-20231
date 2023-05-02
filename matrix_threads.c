#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"
/*
Para compilar este ejemplo:  gcc -o exe  matrix.c matrix_threads.c
1. validaciones en main
2. creacion de hilos o secuencial
3. invocar operación
*/

int main()
{

    Matrix *M = create_matrix(3, 2);
    int p = 1;
    for (int i = 0; i < M->rows; i++)
    {
        for (int j = 0; j < M->cols; j++)
        {
            M->elements[i][j] = p;
            p++;
        }
    }
    // Matrix *N = create_matrix(3, 6);
    // for (int i = 0; i < N->rows; i++)
    // {
    //     for (int j = 0; j < N->cols; j++)
    //     {
    //         N->elements[i][j] = 2;
    //     }
    // }

    // Matrix *M = create_matrix(3, 2);
    // M->elements[0][0] = 1.0;
    // M->elements[0][1] = 2.0;
    // M->elements[1][0] = 4.0;
    // M->elements[1][1] = 8.0;
    // M->elements[2][0] = 5.0;
    // M->elements[2][1] = 6.0;
    Matrix *M2 = create_matrix(2, 3);
    M2->elements[0][0] = 7.0;
    M2->elements[0][1] = 8.0;
    M2->elements[0][2] = 9.0;
    M2->elements[1][0] = 1.0;
    M2->elements[1][1] = 2.0;
    M2->elements[1][2] = 3.0;
    Matrix *N = create_matrix(3, 2);
    N->elements[0][0] = 1.0;
    N->elements[0][1] = 2.0;
    N->elements[1][0] = 3.0;
    N->elements[1][1] = 4.0;
    N->elements[2][0] = 5.0;
    N->elements[2][1] = 6.0;

    Matrix *O = create_matrix(3, 6);
    p = 1;
    for (int i = 0; i < O->rows; i++)
    {
        for (int j = 0; j < O->cols; j++)
        {
            O->elements[i][j] = p;
            p++;
        }
    }

    // Vector *r = init_vector_threads(M, 2, 6);
    // print_vector(r);
    print_matrix(M);
    init_matrix_threads_void(M, 2, 3, 6.0);

    // Matrix *result = init_matrix_threads(M, N, 2, 1);
    //  print_matrix(result);

    // result = init_matrix_threads(M2, M, 2, 2);
    // print_matrix(result);

    // print_matrix(M);
    print_matrix(M);
    // print_matrix(M2);

    // Liberar la memoria
    // free_vector(r);
    free_matrix(M);
    free_matrix(N);
    free_matrix(O);
    // free_matrix(result);
    return 0;
}