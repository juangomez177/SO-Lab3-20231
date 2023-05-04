#include "matrix.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

Matrix *R;

Vector *create_vector(int rows, int cols)
{
    Vector *v = calloc(1, sizeof(Vector));
    v->rows = rows;
    v->cols = cols;
    v->elements = calloc(rows + cols - 1, sizeof(double));
    return v;
}

Matrix *create_matrix(int rows, int cols)
{
    Matrix *M = malloc(sizeof(Matrix));
    M->rows = rows;
    M->cols = cols;
    M->elements = calloc(rows, sizeof(double *));
    for (int i = 0; i < rows; ++i)
    {
        M->elements[i] = calloc(cols, sizeof(double));
    }
    return M;
}

/*

Vector *create_vector_from_file(const char *file_path, int rows, int cols)
{
    Vector *v = create_vector(rows, cols);
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr,
                "Failed to open \"%s\". Not possible to create vector from file.\n",
                file_path);
        return NULL;
    }

    double d;
    for (int i = 0; i < v->rows + v->cols - 1; i++)
    {
        const int r = fscanf(fp, "%lf", &d);
        if (r != 1)
        {
            fprintf(stderr, "fscanf failed.\n");
            fclose(fp);
            return NULL;
        }
        v->elements[i] = d;
    }

    fclose(fp);
    return v;
}

*/

Matrix *create_matrix_from_file(const char *file_path, int rows, int cols)
{
    Matrix *M = create_matrix(rows, cols);
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr,
                "Failed to open \"%s\". Not possible to create matrix from file.\n",
                file_path);
        return NULL;
    }

    double d;
    for (int i = 0; i < M->rows; ++i)
    {
        for (int j = 0; j < M->cols; ++j)
        {
            const int r = fscanf(fp, "%lf", &d);
            if (r != 1)
            {
                fprintf(stderr, "fscanf failed.\n");
                fclose(fp);
                return NULL;
            }
            M->elements[i][j] = d;
        }
    }

    fclose(fp);
    return M;
}

void init_vector_rand(Vector *V)
{
    for (int i = 0; i < V->rows + V->cols - 1; ++i)
    {
        V->elements[i] = (double)rand() / (double)RAND_MAX;
    }
}

void init_matrix_rand(Matrix *M)
{
    for (int i = 0; i < M->rows; ++i)
    {
        for (int j = 0; j < M->cols; ++j)
        {
            M->elements[i][j] = (double)rand() / (double)RAND_MAX;
        }
    }
}

/*
void copy_vector(Vector *dst, const Vector *src)
{
    for (int i = 0; i < src->rows + src->cols - 1; ++i)
    {
        dst->elements[i] = src->elements[i];
    }
}
*/
void copy_matrix(Matrix *dst, const Matrix *src)
{
    for (int i = 0; i < src->rows; ++i)
    {
        for (int j = 0; j < src->cols; ++j)
        {
            dst->elements[i][j] = src->elements[i][j];
        }
    }
}

void free_vector(Vector *v)
{
    free(v->elements);
    free(v);
}

void free_matrix(Matrix *M)
{
    for (int i = 0; i < M->rows; ++i)
    {
        free(M->elements[i]);
    }
    free(M->elements);
    free(M);
}

// Punto 1 del taller
void *Matrix_col_mean(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    for (int i = data->thread_id; i < data->V->cols; i += data->thread_count)
    {
        double sum = 0.0;
        for (int j = 0; j < data->M->rows; ++j)
        {
            sum += data->M->elements[j][i];
        }
        data->V->elements[i] = sum / data->M->rows;
    }
}

// Punto 2 del taller
void *Matrix_col_vrz(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    Vector *r = init_vector_threads(data->M, data->thread_count, 1);
    Vector *v = create_vector(1, data->M->cols);
    Matrix *N = create_matrix(data->M->rows, data->M->cols);

    for (int i = data->thread_id; i < data->M->cols; i += data->thread_count)
    {
        for (int j = 0; j < data->M->rows; ++j)
        {
            N->elements[j][i] = data->M->elements[j][i] - r->elements[i];
            N->elements[j][i] *= N->elements[j][i];
        }
    }
    v = init_vector_threads(N, data->thread_count, 6);
    for (int i = data->thread_id; i < v->rows + v->cols - 1; i += data->thread_count)
    {
        data->V->elements[i] = v->elements[i] / N->rows;
    }
}

// Punto 3 del taller
void *Matrix_col_std(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;
    Vector *v = init_vector_threads(data->M, data->thread_count, 2); // matrix_col_vrz(data->M);

    for (int i = data->thread_id; i < data->V->rows + data->V->cols - 1; i += data->thread_count)
    {
        data->V->elements[i] = sqrt(v->elements[i]);
    }
}

// Punto 3 del taller, calculo de la raiz caudrada
double sqrt(double number)
{
    float temp, sqrt;

    // store the half of the given number e.g from 256 => 128
    sqrt = number / 2;
    temp = 0;

    // Iterate until sqrt is different of temp, that is updated on the loop
    while (sqrt != temp)
    {
        // initially 0, is updated with the initial value of 128
        // (on second iteration = 65)
        // and so on
        temp = sqrt;

        // Then, replace values (256 / 128 + 128 ) / 2 = 65
        // (on second iteration 34.46923076923077)
        // and so on
        sqrt = (number / temp + temp) / 2;
    }
    return sqrt;
}

// Punto 4 del taller
void *Matrix_col_min(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    for (int i = data->thread_id; i < data->V->cols; i += data->thread_count)
    {
        double min = data->M->elements[0][i];
        for (int j = 0; j < data->M->rows; ++j)
        {
            if (min > data->M->elements[j][i])
            {
                min = data->M->elements[j][i];
            }
        }
        data->V->elements[i] = min;
    }
}

// Punto 4 del taller
void *Matrix_col_max(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    for (int i = data->thread_id; i < data->V->cols; i += data->thread_count)
    {
        double max = data->M->elements[0][i];
        for (int j = 0; j < data->M->rows; ++j)
        {
            if (max < data->M->elements[j][i])
            {
                max = data->M->elements[j][i];
            }
        }
        data->V->elements[i] = max;
    }
}

// Punto 5 del taller
void *Add_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    // Calcula el tamaño de la sección que cada hilo sumará
    // printf("thread id: %d\n", data->thread_id);
    R = create_matrix(data->result->rows, data->result->cols);

    // Suma las secciones correspondientes
    for (int i = data->thread_id; i < data->result->cols; i += data->thread_count)
    {
        for (int j = 0; j < data->result->rows; j++)
        {
            data->result->elements[j][i] = data->M->elements[j][i] + data->N->elements[j][i];
            R->elements[j][i] = data->result->elements[j][i];
        }
    }

    return NULL;
}

void *Matrix_col_sum(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    for (int i = data->thread_id; i < data->M->cols; i += data->thread_count)
    {
        double sum = 0.0;
        for (int j = 0; j < data->M->rows; ++j)
        {
            sum += data->M->elements[j][i];
        }
        data->V->elements[i] = sum;
    }
}

// Punto 6 del taller
void *Dot_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    R = create_matrix(data->result->rows, data->result->cols);

    // Calcula el tamaño de la sección que cada hilo multiplicará
    for (int escalar = data->thread_id; escalar < data->result->cols; escalar += data->thread_count) // cambia cols de N y cols de r
    {
        for (int i = 0; i < data->M->rows; i++) // cambia rows de M y rows de r
        {
            double d = 0.0;
            for (int j = 0; j < data->N->rows; j++) // cambia cols de M y rows de N
            {
                d += data->M->elements[i][j] * data->N->elements[j][escalar];
            }
            data->result->elements[i][escalar] = d;
            R->elements[i][escalar] = data->result->elements[i][escalar];
        }
    }
    return NULL;
}

// Punto 7 del taller
void *Scalar_matrix(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;

    for (int i = data->thread_id; i < data->M->cols; i += data->thread_count)
    {
        for (int j = 0; j < data->M->rows; ++j)
        {
            data->M->elements[j][i] *= data->escalar;
        }
    }
}

// Punto 8 del taller
void *Matrix_col_normalized_min_max(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;
    Vector *min = init_vector_threads(data->M, data->thread_count, 4); // matrix_col_min(M);
    Vector *max = init_vector_threads(data->M, data->thread_count, 5); // matrix_col_max(M);

    // Normalizamos la matriz utilizando el método de Min-Max
    for (int i = data->thread_id; i < data->M->cols; i += data->thread_count)
    {
        for (int j = 0; j < data->M->rows; j++)
        {
            data->M->elements[j][i] = (data->M->elements[j][i] - min->elements[i]) / (max->elements[i] - min->elements[i]);
        }
    }
}

// Punto 9 del taller
void *Matrix_col_normalized_standard_score(void *thread_arg)
{
    Thread_data *data = (Thread_data *)thread_arg;
    Vector *mean = init_vector_threads(data->M, data->thread_count, 1); // matrix_col_mean(M);
    Vector *std_dev = init_vector_threads(data->M, data->thread_count, 3);

    // Normalization of each column
    for (int i = data->thread_id; i < data->M->cols; i += data->thread_count)
    {
        // Normalize each value
        for (int j = 0; j < data->M->rows; j++)
        {
            data->M->elements[j][i] = (data->M->elements[j][i] - mean->elements[i]) / std_dev->elements[i];
        }
    }
}

// Metodo que maneja los hilos para puntos 1, 2, 3 y 4. Genera un vector como resultado
Vector *init_vector_threads(const Matrix *M, int thread_count, int operation)
{
    Vector *result = create_vector(1, M->cols); // malloc(sizeof(Vector));

    if (thread_count > result->cols)
    {
        fprintf(stderr, "Número de hilos inválido: %d > numero de columnas de la matrix resultado: %d\n", thread_count, result->rows);
        free_vector(result);
        exit(0);
    }

    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    Thread_data *thread_args = malloc(thread_count * sizeof(Thread_data));

    // Crea los hilos
    for (int thread = 0; thread < thread_count; thread++)
    {
        thread_args[thread].thread_id = thread;
        thread_args[thread].thread_count = thread_count;
        thread_args[thread].M = M;
        thread_args[thread].V = result;

        switch (operation)
        {
        case 1:
            if (thread_count == 1)
            {
                Matrix_col_mean((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_mean, (void *)&thread_args[thread]);
            }
            break;
        case 2:
            if (thread_count == 1)
            {
                Matrix_col_vrz((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_vrz, (void *)&thread_args[thread]);
            }
            break;
        case 3:
            if (thread_count == 1)
            {
                Matrix_col_std((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_std, (void *)&thread_args[thread]);
            }
            break;
        case 4:
            if (thread_count == 1)
            {
                Matrix_col_min((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_min, (void *)&thread_args[thread]);
            }
            break;
        case 5:
            if (thread_count == 1)
            {
                Matrix_col_max((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_max, (void *)&thread_args[thread]);
            }
            break;
        case 6:
            if (thread_count == 1)
            {
                Matrix_col_sum((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_sum, (void *)&thread_args[thread]);
            }
            break;

        default:
            break;
        }
    }

    // Espera a que finalicen los hilos
    if (thread_count != 1)
    {
        for (int thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
    }

    free(thread_handles);
    free(thread_args);
    return result;
}

// Metodo que maneja los hilos para puntos 5 y 6. Genera una matriz como resultado
Matrix *init_matrix_threads(const Matrix *M, const Matrix *N, int thread_count, int operation)
{
    Matrix *result = create_matrix(M->rows, N->cols);
    if (operation == 5)
    {
        if (M->rows != N->rows || M->cols != N->cols)
        {
            fprintf(stderr, "Dimensiones invalidas para suma de amtrices. (%d, %d) & (%d, %d)\n", M->rows, M->cols,
                    N->rows, N->cols);
            free_matrix(result);
            exit(0); // No se pueden sumar matrices de diferentes tamaños
        }
    }
    else
    {
        if (M->cols != N->rows)
        {
            fprintf(stderr, "Dimensiones invalidas. (%d, %d) & (%d, %d)\n", M->rows, M->cols,
                    N->rows, N->cols);
            free_matrix(result);
            return NULL; // No se pueden multiplicar matrices con estas dimensiones
        }
    }
    if (thread_count > result->cols)
    {
        fprintf(stderr, "Número de hilos inválido: %d > numero de columnas de la matrix resultado: %d\n", thread_count, result->cols);
        free_matrix(result);
        return NULL;
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

        if (operation == 5)
        {
            if (thread_count == 1)
            {
                Add_matrix((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Add_matrix, (void *)&thread_args[thread]);
            }
        }
        else
        {
            if (thread_count == 1)
            {
                Dot_matrix((void *)&thread_args[thread]);
            }
            else
            {

                pthread_create(&thread_handles[thread], NULL, Dot_matrix, (void *)&thread_args[thread]);
            }
        }
    }

    // Espera a que finalicen los hilos
    if (thread_count > 1)
    {
        for (int thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
    }

    free(thread_handles);
    free(thread_args);

    return result;
}

// Metodo que maneja los hilos para puntos 7, 8 y 9. Modifican la matriz no hay estructura resultado
Matrix *init_matrix_threads_void(Matrix *M, int thread_count, int operation, double escalar)
{

    if (thread_count > M->cols)
    {
        fprintf(stderr, "Número de hilos inválido: %d > numero de columnas de la matrix resultado: %d\n", thread_count, M->cols);
        exit(0);
    }
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    Thread_data *thread_args = malloc(thread_count * sizeof(Thread_data));

    for (int thread = 0; thread < thread_count; thread++)
    {
        thread_args[thread].thread_id = thread;
        thread_args[thread].thread_count = thread_count;
        thread_args[thread].M = M;
        thread_args[thread].escalar = escalar;

        switch (operation)
        {
        case 7:
            if (thread_count == 1)
            {
                Scalar_matrix((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Scalar_matrix, (void *)&thread_args[thread]);
            }
            break;
        case 8:
            if (thread_count == 1)
            {
                Matrix_col_normalized_min_max((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_normalized_min_max, (void *)&thread_args[thread]);
            }
            break;
        case 9:
            if (thread_count == 1)
            {

                Matrix_col_normalized_standard_score((void *)&thread_args[thread]);
            }
            else
            {
                pthread_create(&thread_handles[thread], NULL, Matrix_col_normalized_standard_score, (void *)&thread_args[thread]);
            }
            break;
        }
    }

    if (thread_count != 1)
    {
        for (int thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
    }

    free(thread_handles);
    free(thread_args);

    return M;
}

/*
Vector *add_vector(const Vector *a, const Vector *b)
{
    if ((a->rows + a->cols - 1) != (b->rows + b->cols - 1))
    {
        fprintf(stderr, "Invalid size. %d and %d\n", a->rows + a->cols - 1,
                b->rows + b->cols - 1);
        return NULL;
    }

    Vector *r = create_vector(a->rows, a->cols);
    for (int i = 0; i < r->rows + r->cols - 1; ++i)
    {
        r->elements[i] = a->elements[i] + b->elements[i];
    }
    return r;
}

void scalar_vector(Vector *V, double escalar)
{
    for (int i = 0; i < V->rows + V->cols - 1; ++i)
    {
        V->elements[i] *= escalar;
    }
}

*/

void print_vector(const Vector *v)
{
    if (v == NULL)
    {

        printf("########## VECTOR VACÍ0 ############ \n");
    }
    else
    {

        if (v->rows != 1)
        {
            printf("size=%d\n", v->rows + v->cols - 1);
            for (int i = 0; i < v->rows + v->cols - 1; ++i)
            {
                printf("[%lf]\n", v->elements[i]);
            }
        }
        else
        {
            printf("size=%d, [", v->rows + v->cols - 1);
            for (int i = 0; i < v->rows + v->cols - 1; ++i)
            {
                printf("%lf ", v->elements[i]);
            }
            printf("]\n");
        }
    }
}

void print_matrix(const Matrix *M)
{

    if (M == NULL)
    {
        printf("########## MATRIZ VACÍA ############ \n");
    }
    else
    {
        printf("rows = %d,cols = %d\n", M->rows, M->cols);
        for (int i = 0; i < M->rows; ++i)
        {
            printf("[");
            for (int j = 0; j < M->cols; ++j)
            {
                printf("%lf ", M->elements[i][j]);
            }
            printf("]\n");
        }
    }
}
