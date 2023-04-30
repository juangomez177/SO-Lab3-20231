#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "matrix.h"


int thread_count;
struct timeval t_start, t_end;
double exec_time;
int option;                          //Número de Operación a realizar
int rows1, cols1;                       //Número de fila y columnas respectivamente del primer operando
int rows2, cols2;                       //Número de fila y columnas respectivamente del segundo operando
thread_count = 10;              // Inicializando número de hilos


//Función que gestiona los flags
int menu(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, ":o:f:c:r:s:p:")) != -1)
    {
        switch (opt)
        {
            case 'o':
                printf("Operación a realizar: %s\n", optarg);
                option = strtol(optarg, NULL, 10);
                assert(option > 1 && option <= 9);
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
            case ':':
                printf("option -%c needs a value\n", optopt);
                break;
            case '?':
                fprintf(stderr, "Usage: %s  [-o <Operación a realizar>] [-f <fila primer operador>] [-c <columna primer operador>] \
                        [-r <fila segundo operador>] [-s <columna segundo operador>] [-p <Path de archivos con operandos>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("o = %d, f = %d, c = %d, r = %d, s = %d\n", option, rows1, cols1, rows2, cols2);
    return 0;
}

void parallelizationOperateOneMatrix( void (*function), long H)
{

}



int main(int argc, char *argv[])
{

    // Generar las matrices aleatorias
    int **mat1 = generate_random_matrix(rows1, cols1);
    int **mat2 = generate_random_matrix(rows2, cols2);


    long thread;                                                //Inicializando iterador 
    pthread_t *thread_handles;                                  //Inicializando el manejador de hilo
    thread_handles = malloc(thread_count * sizeof(pthread_t));


    //Se debe hacer una selección de creación de hilos de acuerdo si se opera 1 o dos matrices
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       parallelizationOperateOneMatrix, (void *)thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    gettimeofday(&t_start, NULL);

    gettimeofday(&t_end, NULL);

    Matrix *M = create_matrix(3, 2);
    M->elements[0][0] = 1.0;
    M->elements[0][1] = 2.0;
    M->elements[1][0] = 4.0;
    M->elements[1][1] = 8.0;
    M->elements[2][0] = 5.0;
    M->elements[2][1] = 6.0;

    Matrix *M2 = create_matrix(2, 3);
    M2->elements[0][0] = 7.0;
    M2->elements[0][1] = 8.0;
    M2->elements[0][2] = 9.0;
    M2->elements[1][0] = 1.0;
    M2->elements[1][1] = 2.0;
    M2->elements[1][2] = 3.0;

    Matrix *M3 = create_matrix(3, 2);
    M3->elements[0][0] = 1.0;
    M3->elements[0][1] = 2.0;
    M3->elements[1][0] = 3.0;
    M3->elements[1][1] = 4.0;
    M3->elements[2][0] = 5.0;
    M3->elements[2][1] = 6.0;

    Matrix *M4 = create_matrix(3, 2);

    Vector *v = create_vector(1, 3);
    v->elements[0] = 1.0;
    v->elements[1] = 2.0;
    v->elements[2] = 3.0;

    Vector *v2 = create_vector(1, 3);
    init_vector_rand(v2);

    free_matrix(M);
    free_matrix(M2);
    free_matrix(M3);
    free_matrix(M4);
    free_vector(v);
    free_vector(v2);
    // free_vector(result);


    


    return 0;
}