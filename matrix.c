#include "matrix.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <math.h>

Vector *create_vector(int rows, int cols)
{
    Vector *v = calloc(1, sizeof(Vector));
    v->rows = rows;
    v->cols = cols;
    v->elements = calloc(rows + cols - 1, sizeof(double));
    return v;
}

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

void init_vector_rand(Vector *V)
{
    for (int i = 0; i < V->rows + V->cols - 1; ++i)
    {
        V->elements[i] = (double)rand() / (double)RAND_MAX;
    }
}

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

void copy_vector(Vector *dst, const Vector *src)
{
    for (int i = 0; i < src->rows + src->cols - 1; ++i)
    {
        dst->elements[i] = src->elements[i];
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

// Vector *dot_vector_matrix(const Vector *v, const Matrix *M)
// {
//     if (v->size != M->rows)
//     {
//         fprintf(stderr, "Invalid size. %d and (%d, %d)\n", v->size, M->rows, M->cols);
//         return NULL;
//     }

//     Vector *r = create_vector(M->cols);
//     for (int i = 0; i < r->size; ++i)
//     {
//         double d = 0.0;
//         for (int j = 0; j < M->rows; ++j)
//         {
//             d += (v->elements[j] * M->elements[j][i]);
//         }
//         r->elements[i] = d;
//     }

//     return r;
// }

Matrix *add_matrix(const Matrix *M, const Matrix *N)
{
    if (M->rows != N->rows || M->cols != N->cols)
    {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols,
                N->rows, N->cols);
        return NULL;
    }

    Matrix *R = create_matrix(M->rows, M->cols);
    for (int i = 0; i < M->rows; ++i)
    {
        for (int j = 0; j < M->cols; ++j)
        {
            R->elements[i][j] = M->elements[i][j] + N->elements[i][j];
        }
    }
    return R;
}

Matrix *dot_matrix(const Matrix *M, const Matrix *N)
{
    if (M->cols != N->rows)
    {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols,
                N->rows, N->cols);
        return NULL;
    }

    Matrix *A = create_matrix(M->rows, N->cols);
    for (int i = 0; i < M->rows; ++i)
    {
        for (int j = 0; j < N->cols; ++j)
        {
            double d = 0.0;
            for (int k = 0; k < M->cols; ++k)
            {
                d += M->elements[i][k] * N->elements[k][j];
            }
            A->elements[i][j] = d;
        }
    }

    return A;
}


Vector *matrix_col_mean(const Matrix *M)
{
    Vector *V = create_vector(1, M->cols);

    for (int i = 0; i < M->cols; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j)
        {
            sum += M->elements[j][i];
        }
        V->elements[i] = sum / M->rows;
    }

    return V;
}

Vector *matrix_col_sum(const Matrix *M)
{
    // Revisar esta parte
    Vector *v = create_vector(1, M->cols);

    for (int i = 0; i < M->cols; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j)
        {
            sum += M->elements[j][i];
        }
        v->elements[i] = sum;
    }

    return v;
}

Vector *matrix_col_max(const Matrix *M)
{
    Vector *v = create_vector(1, M->cols);
    for (int i = 0; i < M->cols; ++i)
    {
        double max = M->elements[0][i];
        for (int j = 0; j < M->rows; ++j)
        {
            if (max < M->elements[j][i])
            {
                max = M->elements[j][i];
            }
        }
        v->elements[i] = max;
    }
    return v;
}

Vector *matrix_col_min(const Matrix *M)
{
    Vector *v = create_vector(1, M->cols);
    for (int i = 0; i < M->cols; ++i)
    {
        double min = M->elements[0][i];
        for (int j = 0; j < M->rows; ++j)
        {
            if (min > M->elements[j][i])
            {
                min = M->elements[j][i];
            }
        }
        v->elements[i] = min;
    }
    return v;
}

Vector *matrix_col_vrz(const Matrix *M)
{
    Vector *r = matrix_col_mean(M);
    Vector *v = create_vector(1, M->cols);
    Matrix *N = create_matrix(M->rows, M->cols);

    for (int i = 0; i < M->cols; ++i)
    {
        for (int j = 0; j < M->rows; ++j)
        {
            N->elements[j][i] = M->elements[j][i] - r->elements[i];
            N->elements[j][i] *= N->elements[j][i];
        }
    }
    v = matrix_col_sum(N);
    for (int i = 0; i < v->rows + v->cols - 1; ++i)
    {
        v->elements[i] = v->elements[i] / N->rows;
    }
    return v;
}

Vector *matrix_col_std(const Matrix *M)
{
    Vector *v = matrix_col_vrz(M);
    for (int i = 0; i < v->rows + v->cols - 1; ++i)
    {
        v->elements[i] = sqrt(v->elements[i]);
    }
    return v;
}

void scalar_matrix(Matrix *M, double k)
{
    for (int i = 0; i < M->rows; ++i)
    {
        for (int j = 0; j < M->cols; ++j)
        {
            M->elements[i][j] *= k;
        }
    }
}

void scalar_vector(Vector *V, double k)
{
    for (int i = 0; i < V->rows + V->cols - 1; ++i)
    {
        V->elements[i] *= k;
    }
}

void matrix_col_normalize_standard_score(Matrix *M)
{
    Vector *mean = matrix_col_mean(M);
    Vector *std_dev = matrix_col_std(M);

    // Normalization of each column
    for (int i = 0; i < M->cols; i++)
    {
        // Normalize each value
        for (int j = 0; j < M->rows; j++)
        {
            M->elements[j][i] = (M->elements[j][i] - mean->elements[i]) / std_dev->elements[i];
        }
    }
}

void matrix_col_normalize_min_max(Matrix *M)
{
    Vector *min = matrix_col_min(M);
    Vector *max = matrix_col_max(M);

    // Normalizamos la matriz utilizando el método de Min-Max
    for (int i = 0; i < M->cols; i++)
    {
        for (int j = 0; j < M->rows; j++)
        {
            M->elements[j][i] = (M->elements[j][i] - min->elements[i]) / (max->elements[i] - min->elements[i]);
        }
    }
}

void print_vector(const Vector *v)
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

void print_matrix(const Matrix *M)
{
    printf("rows=%d,cols=%d\n", M->rows, M->cols);
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
