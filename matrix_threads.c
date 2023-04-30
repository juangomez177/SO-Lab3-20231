#include "matrix.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*
Para compilar este ejemplo:  gcc -o exe  matrix.c matrix_threads.c
*/
typedef struct
{
    int thread_id;
    int thread_count;
    const Matrix *M;
    const Matrix *N;
    Matrix *result;
} Thread_data;

void *Add_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;
    printf("thread id: %d\n", data->thread_id);

    // Calcula el tamaño de la sección que cada hilo sumará
    int section_size = data->result->cols / data->thread_count;
    int cols_extra = data->result->cols % data->thread_count;
    // int start_col = data->thread_id * section_size;
    // int end_col = start_col + section_size;

    // puts("M:");
    // print_matrix(data->M);
    // puts("N:");
    // print_matrix(data->N);
    int start_col = data->thread_id * section_size + (data->thread_id < cols_extra ? data->thread_id : cols_extra); //Revisar al final de este archivo para ver la forma extendida
    int end_col = start_col + section_size - 1 + (data->thread_id < cols_extra ? 1 : 0); //Revisar al final de este archivo para ver la forma extendida

    printf("section_size: %d\n", section_size);
    printf("cols_extra: %d\n", cols_extra);
    printf("start_col: %d\n", start_col);
    printf("end_col: %d\n", end_col);
    puts("");

    // Suma las secciones correspondientes
    for (int j = start_col; j <= end_col; j++)
    {
        for (int i = 0; i < data->result->rows; i++)
        {
            printf("M element[%d][%d]: %f\n", i, j, data->M->elements[i][j]);
            printf("N element[%d][%d]: %f\n", i, j, data->N->elements[i][j]);
            data->result->elements[i][j] = data->M->elements[i][j] + data->N->elements[i][j];
        }
    }
    // puts("");
    // print_matrix(data->result);
    // puts("");
    return NULL;
}

Matrix *Add_matrix_threads(const Matrix *M, const Matrix *N, int thread_count)
{
    if (M->rows != N->rows || M->cols != N->cols)
    {
        return NULL; // No se pueden sumar matrices de diferentes tamaños
    }

    Matrix *result = malloc(sizeof(Matrix));
    result->rows = M->rows;
    result->cols = M->cols;
    result->elements = malloc(result->rows * sizeof(double *));
    for (int i = 0; i < result->rows; i++)
    {
        result->elements[i] = malloc(result->cols * sizeof(double));
    }

    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    Thread_data *thread_args = malloc(thread_count * sizeof(Thread_data));

    // Crea los hilos
    for (int thread = 0; thread < thread_count; thread++)
    {
        thread_args[thread].thread_id = thread;
        thread_args[thread].thread_count = thread_count;
        thread_args[thread].M = M;
        thread_args[thread].N = N;
        thread_args[thread].result = result;

        pthread_create(&thread_handles[thread], NULL, Add_matrix, (void *)&thread_args[thread]);
    }

    // Espera a que finalicen los hilos
    for (int thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    free(thread_handles);
    free(thread_args);

    return result;
}

int main()
{

    Matrix *M = create_matrix(6, 3);
    for (int i = 0; i < M->rows; i++)
    {
        for (int j = 0; j < M->cols; j++)
        {
            M->elements[i][j] = 1;
        }
    }
    Matrix *N = create_matrix(6, 3);
    for (int i = 0; i < N->rows; i++)
    {
        for (int j = 0; j < N->cols; j++)
        {
            N->elements[i][j] = 2;
        }
    }

    Matrix *result = Add_matrix_threads(M, N, 3);

    print_matrix(result);

    // Libera la memoria
    free_matrix(M);
    free_matrix(N);
    free_matrix(result);

    return 0;
}
/*
int start_col, end_col;
if (data->thread_id < cols_extra)
{
    // Si el hilo está entre los primeros hilos "columnas_sobrantes",
    // se le asigna una columna adicional
    int columnas_por_hilo_con_adicional = section_size + 1;
    start_col = data->thread_id * columnas_por_hilo_con_adicional;
}
else
{
    // Si el hilo no está entre los primeros hilos"columnas_sobrantes",
    // no se le asigna una columna adicional
    start_col = cols_extra * (section_size + 1) +
                (data->thread_id - cols_extra) * section_size;
}

// Una vez que se ha determinado la columna inicial del hilo,
// se calcula su rango completo de columnas
end_col = start_col + section_size - 1;
if (data->thread_id < cols_extra)
{
    // Si el hilo está entre los primeros hilos "columnas_sobrantes",
    // se le asignó una columna adicional, por lo que se debe sumar 1 al rango
    end_col++;
}*/