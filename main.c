#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include "matrix.h"

int thread_count;
struct timeval t_start, t_end;
double exec_time;
int option;       // Número de Operación a realizar
int hilos;        // Número de hilos
double escalar;   // Número escalar
int rows1, cols1; // Número de fila y columnas respectivamente del primer operando
int rows2, cols2; // Número de fila y columnas respectivamente del segundo operando

// Función que gestiona los flags
int Menu_flags(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, ":h:e:f:c:r:s:p:o:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printf("Número de hilos: %s\n", optarg);
            hilos = strtol(optarg, NULL, 10);
            assert(hilos > 1 && hilos <= 64);
            break;
        case 'e':
            char *endptr;
            printf("Número escalar: %s\n", optarg);
            double escalar = strtod(optarg, &endptr);
            break;
        case 'f':
            printf("Número de fila del primer operando: %s\n", optarg);
            rows1 = strtol(optarg, NULL, 10);
            assert(rows1 > 1 && rows1 <= 2147483647);
            break;
        case 'c':
            printf("Número de columnas del primer operando: %s\n", optarg);
            cols1 = strtol(optarg, NULL, 10);
            assert(cols1 > 1 && cols1 <= 2147483647);
            break;
        case 'r':
            printf("Número de fila del segundo operando: %s\n", optarg);
            rows2 = strtol(optarg, NULL, 10);
            assert(rows2 > 1 && rows2 <= 2147483647);
            break;
        case 's':
            printf("Número de columnas del segundo operando: %s\n", optarg);
            cols2 = strtol(optarg, NULL, 10);
            assert(cols2 > 1 && cols2 <= 2147483647);
            break;
        case 'p':
            printf("Path del archivo con los operandos de la operación a realizar: %s\n", optarg);
            break;
        case 'o':
            option = strtol(optarg, NULL, 10);
            printf("Operación a realizar: %s\n", optarg);
            assert(option >= 1 && option <= 9);
            break;
        case ':':
            printf("option -%c needs a value\n", optopt);
            break;
        case '?':
            fprintf(stderr, "Usage: %s  [-h <número de hilos>] [-e <escalar>] [-f <fila primer operador>] [-c <columna primer operador>] \
                        [-r <fila segundo operador>] [-s <columna segundo operador>] [-o <Operación a realizar>] [-p <Path de archivos con operandos>]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    //printf("h = %d, e = %d, f = %d, c = %d, r = %d, s = %d, o = %d\n", hilos, escalar, rows1, cols1, rows2, cols2, option);
    return 0;
}

void Menu_operations(int h, int option, double e, int f, int c, int r, int s, Matrix *A, Matrix *B)
{
    switch (option)
    {
    case 1:
        printf("Calculando la media de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        gettimeofday(&t_start, NULL);
            init_vector_threads(A, h, 1);
        gettimeofday(&t_end, NULL);
        break;
    case 2:
        printf("Calculando la varianza de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        gettimeofday(&t_start, NULL);
        init_vector_threads(A, h, 2);
        gettimeofday(&t_end, NULL);
        break;
    case 3:
        printf("Calculando la variación estandar de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        gettimeofday(&t_start, NULL);
        init_vector_threads(A, h, 3);
        gettimeofday(&t_end, NULL);
        break;
    case 4:
        printf("Calculando el valor minimo y el valor maximo de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        gettimeofday(&t_start, NULL);
        init_vector_threads(A, h, 4);
        gettimeofday(&t_end, NULL);
        break;
    case 5:
        printf("Calculando la suma de la matriz A(fxc): %dx%d con la matriz B(rxs): '%dx%d\n'", f, c, r, s);
        gettimeofday(&t_start, NULL);
        init_matrix_threads(A, B, h, 5);
        gettimeofday(&t_end, NULL);
        break;
    case 6:
        printf("Calculando el producto punto de la matriz B(fxc): %dx%d con la matriz B(rxs): '%dx%d\n'", f, c, r, s);
        gettimeofday(&t_start, NULL);
        init_matrix_threads(A, B, h, 6);
        gettimeofday(&t_end, NULL);
        break;
    case 7:
        printf("Calculando el producto de un escalar %d con la matriz A(fxc): '%dx%d\n'", e, f, c);
        gettimeofday(&t_start, NULL);
        init_matrix_threads_void(A, h, 7, e);
        gettimeofday(&t_end, NULL);
        break;
    case 8:
        printf("Calculando la normalización (Opción 1) de las columnas de la matriz A(fxc): '%dx%d\n'", f, c);
        gettimeofday(&t_start, NULL);
        init_matrix_threads_void(A, h, 8, e);
        gettimeofday(&t_end, NULL);
        break;
    case 9:
        printf("Calculando la normalización (Opción 2) de las columnas de la matriz A(fxc): '%dx%d\n'", f, c);
        gettimeofday(&t_start, NULL);
        init_matrix_threads_void(A, h, 9, e);
        gettimeofday(&t_end, NULL);
        break;
    default:
        printf("Esta opción no crresponde con ninguna de las opciones de las operaciones que este programa puede ejecutar");
    }
    
}

int main(int argc, char *argv[])
{
    Menu_flags(argc, argv);

    Matrix *M = create_matrix(rows1, cols1);
    Matrix *N = create_matrix(rows2, cols2);

    init_matrix_rand(M);
    init_matrix_rand(N);

    Menu_operations(hilos, option, escalar, rows1, cols1, rows2, cols2, M, N);

    print_matrix(R);

    exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
    exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
    printf("Tiempo de ejecución: %f ms \n", exec_time);


    // Libera la memoria
    free_matrix(M);
    free_matrix(N);

    return 0;

}