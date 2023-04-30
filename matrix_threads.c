#include "matrix.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*
Para compilar este ejemplo:  gcc -o exe  matrix.c matrix_threads.c
*/
Vector *start_end_cols(Thread_data *data)
{
    int section_size = data->result->cols / data->thread_count;
    int cols_extra = data->result->cols % data->thread_count;
    int start_col = data->thread_id * section_size + (data->thread_id < cols_extra ? data->thread_id : cols_extra); // Revisar al final de este archivo para ver la forma extendida
    int end_col = start_col + section_size - 1 + (data->thread_id < cols_extra ? 1 : 0);                            // Revisar al final de este archivo para ver la forma extendida
    Vector *V = create_vector(1, 2);

    V->elements[0] = start_col;
    V->elements[1] = end_col;

    return V;
}

void *Add_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    // Calcula el tamaño de la sección que cada hilo sumará
    Vector *V = start_end_cols(data);
    int start_col = V->elements[0];
    int end_col = V->elements[1];

    // Suma las secciones correspondientes
    for (int i = start_col; i <= end_col; i++)
    {
        for (int j = 0; j < data->result->rows; j++)
        {
            data->result->elements[j][i] = data->M->elements[j][i] + data->N->elements[j][i];
        }
    }
    return NULL;
}

void *Dot_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    // Calcula el tamaño de la sección que cada hilo multiplicará
    Vector *V = start_end_cols(data);
    int start_col = V->elements[0];
    int end_col = V->elements[1];

    for (int k = start_col; k < end_col; k++)// cambia cols de N
    {
        for (int i = 0; i < data->M->rows; i++)// cambia rows de M
        {
            double d = 0.0;
            for (int j = 0; j < data->N->rows; j++)// cambia cols de M y rows de N
            {
                d += data->M->elements[i][j] * data->N->elements[j][k];
            }
            data->result->elements[i][k] = d;
        }
    }

    // for (int i = 0; i < data->M->rows; ++i) // Este loop itera entre las filas de M
    // {
    //     for (int j = 0; j < data->N->cols; ++j) // Este loop se encarga de iterar las columnas de N
    //     {
    //         double d = 0.0;
    //         for (int k = 0; k < data->M->cols; ++k) // Este loop se encarga de iterar la misma filas de  M y la misma columna de N
    //         {
    //             d += data->M->elements[i][k] * data->N->elements[k][j];
    //         }
    //         data->result->elements[i][j] = d;
    //     }
    // }
    return NULL;
}

Matrix *Add_matrix_threads(const Matrix *M, const Matrix *N, int thread_count, int operation)
{
    Matrix *result = malloc(sizeof(Matrix));
    if (operation == 1)
    {
        if (M->rows != N->rows || M->cols != N->cols)
        {
            fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols,
                    N->rows, N->cols);
            free_matrix(result);
            return NULL; // No se pueden sumar matrices de diferentes tamaños
        }
        result->rows = M->rows;
        result->cols = M->cols;
        result->elements = malloc(result->rows * sizeof(double *));
    }
    else
    {
        if (M->cols != N->rows)
        {
            fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols,
                    N->rows, N->cols);
            free_matrix(result);
            return NULL; // Dimensiones invalidas para realizar producto punto
        }
        result->rows = M->cols;
        result->cols = N->rows;
        result->elements = malloc(result->rows * sizeof(double *));
    }

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
        if(operation == 1){
            pthread_create(&thread_handles[thread], NULL, Add_matrix, (void *)&thread_args[thread]);
        }else{
            pthread_create(&thread_handles[thread], NULL, Dot_matrix, (void *)&thread_args[thread]);
        }
        
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

    Matrix *result = Add_matrix_threads(M, N, 3, 1);

    Matrix *result2 = Add_matrix_threads(M, N, 3, 2);

    print_matrix(result);
    print_matrix(result2);

    // Libera la memoria
    free_matrix(M);
    free_matrix(N);
    free_matrix(result);
    free_matrix(result2);

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