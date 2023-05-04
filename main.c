#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include "matrix.h"
#include <string.h>

int thread_count;
struct timeval t_start, t_end;
double exec_time;
int option;       // Número de Operación a realizar
int hilos = 2;    // Número de hilos
double escalar;   // Número escalar
int rows1, cols1; // Número de fila y columnas respectivamente del primer operando
int rows2, cols2; // Número de fila y columnas respectivamente del segundo operando
Matrix *A;
Matrix *B;
Matrix *result_m;
Vector *result_v;
int is_file = 0; // Para comprobar si la lectura de matrices es por archivo y evitar crear matrices aleatorias

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
            escalar = strtod(optarg, &endptr);
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

            // lectura del archivo
            is_file = 1;

            printf("Archivo: %s\n", optarg);

            FILE *file = fopen(optarg, "r");
            if (!file)
            {
                perror("Error al abrir el archivo");
                return 1;
            }

            char op[10];
            int rows, cols;

            while (fscanf(file, "%s", op) == 1)
            {
                if (strcmp(op, "s") == 0)
                {
                    fscanf(file, "%lf", &escalar);
                }
                else if (strcmp(op, "op1") == 0)
                {
                    fscanf(file, "%d %d", &rows, &cols);
                    Matrix *m = (Matrix *)malloc(sizeof(Matrix));
                    m->rows = rows;
                    m->cols = cols;
                    m->elements = (double **)malloc(rows * sizeof(double *));
                    for (int i = 0; i < rows; i++)
                    {
                        m->elements[i] = (double *)malloc(cols * sizeof(double));
                        for (int j = 0; j < cols; j++)
                        {
                            fscanf(file, "%lf", &(m->elements[i][j]));
                        }
                    }
                    A = m;
                }
                else if (strcmp(op, "op2") == 0)
                {
                    fscanf(file, "%d %d", &rows, &cols);
                    Matrix *m = (Matrix *)malloc(sizeof(Matrix));
                    m->rows = rows;
                    m->cols = cols;
                    m->elements = (double **)malloc(rows * sizeof(double *));
                    for (int i = 0; i < rows; i++)
                    {
                        m->elements[i] = (double *)malloc(cols * sizeof(double));
                        for (int j = 0; j < cols; j++)
                        {
                            fscanf(file, "%lf", &(m->elements[i][j]));
                        }
                    }
                    B = m;
                }
                else
                {
                    printf("Operación no reconocida: %s\n", op);
                }
            }

            fclose(file);

            // Imprime los valores leídos
            printf("scalar: %lf\n", escalar);

            printf("-------------matriz de archivo ----------------\n");

            print_matrix(A);

            printf("--------------matriz 2 de archivo ---------------\n");

            print_matrix(B);
            printf("-----------------------------\n");

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
    // printf("h = %d, e = %d, f = %d, c = %d, r = %d, s = %d, o = %d\n", hilos, escalar, rows1, cols1, rows2, cols2, option);
    return 0;
}

void Menu_operations(int h, int option, double e, int f, int c, int r, int s)
{

    switch (option)
    {
    case 1:
        printf("Calculando la media de cada columna de la matriz A(fxc): %dx%d\n", f, c);

        printf("-------------------Creación de la matriz---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            init_matrix_rand(A);
        }

        print_matrix(A);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, 1, 1);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, h, 1);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);

        break;
    case 2:
        printf("Calculando la varianza de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        printf("-------------------Creación de la matriz---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            init_matrix_rand(A);
        }
        print_matrix(A);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, 1, 2);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, h, 2);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);

        break;
    case 3:
        printf("Calculando la variación estandar de cada columna de la matriz A(fxc): %dx%d\n", f, c);

        printf("-------------------Creación de la matriz---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            init_matrix_rand(A);
        }
        print_matrix(A);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, 1, 3);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, h, 3);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);
        free_matrix(A);

        break;
    case 4:
        printf("-------------------Creación de la matriz---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            init_matrix_rand(A);
        }
        print_matrix(A);

        printf("Calculando el valor minimo y el valor maximo de cada columna de la matriz A(fxc): %dx%d\n", f, c);
        // Secuencial
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, 1, 4);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_v = init_vector_threads(A, h, 4);
        printf("------------VECTOR RESULTANTE---------------\n");
        print_vector(result_v);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);

        break;
    case 5:
        printf("Calculando la suma de la matriz A(fxc): %dx%d con la matriz B(rxs): '%dx%d\n'", f, c, r, s);

        printf("-------------------Creación de las matrices---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            B = create_matrix(rows2, cols2);
            init_matrix_rand(A);
            init_matrix_rand(B);
        }
        print_matrix(A);
        print_matrix(B);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads(A, B, 1, 5);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads(A, B, h, 5);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);
        free_matrix(B);

        break;
    case 6:
        printf("Calculando el producto punto de la matriz B(fxc): %dx%d con la matriz B(rxs): '%dx%d\n'", f, c, r, s);

        printf("-------------------Creación de las matrices---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            B = create_matrix(rows2, cols2);
            init_matrix_rand(A);
            init_matrix_rand(B);
        }
        print_matrix(A);
        print_matrix(B);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads(A, B, 1, 6);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads(A, B, h, 6);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);
        free_matrix(B);

        break;
    case 7:
        printf("Calculando el producto de un escalar %f con la matriz A(fxc): '%dx%d\n'", e, f, c);
        printf("-------------------Creación de las matrices---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            B = create_matrix(rows2, cols2);
            init_matrix_rand(A);
            init_matrix_rand(B);
        }
        print_matrix(A);
        print_matrix(B);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, 1, 7, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, h, 7, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);
        free_matrix(B);

        break;
    case 8:
        printf("Calculando la normalización (Opción 1) de las columnas de la matriz A(fxc): '%dx%d\n'", f, c);
        printf("-------------------Creación de las matrices---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            B = create_matrix(rows2, cols2);
            init_matrix_rand(A);
            init_matrix_rand(B);
        }
        print_matrix(A);
        print_matrix(B);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, 1, 8, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, h, 8, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);
        free_matrix(B);

        break;
    case 9:
        printf("Calculando la normalización (Opción 2) de las columnas de la matriz A(fxc): '%dx%d\n'", f, c);

        printf("-------------------Creación de las matrices---------------\n");
        if (is_file == 0)
        {
            A = create_matrix(rows1, cols1);
            B = create_matrix(rows2, cols2);
            init_matrix_rand(A);
            init_matrix_rand(B);
        }
        print_matrix(A);
        print_matrix(B);

        // Secuencial
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, 1, 9, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución secuencial: %f ms \n", exec_time);

        // Hilos
        gettimeofday(&t_start, NULL);
        result_m = init_matrix_threads_void(A, h, 9, e);
        printf("------------MATRIZ RESULTANTE-----------------\n");
        print_matrix(result_m);

        gettimeofday(&t_end, NULL);

        exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;    // sec to ms
        exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
        printf("Tiempo de ejecución con hilos: %f ms \n", exec_time);

        free_matrix(A);
        free_matrix(B);

        break;
    default:
        printf("Esta opción no corresponde con ninguna de las opciones de las operaciones que este programa puede ejecutar\n");
    }
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        printf("Error, debe ingresar flags\n");
    }
    else
    {

        Menu_flags(argc, argv);
        printf("escalarss %f \n", escalar);

        Menu_operations(hilos, option, escalar, rows1, cols1, rows2, cols2);
    }
    return 0;
}