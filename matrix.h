#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

// Estructura para los argumentos que se pasarán a los hilos
typedef struct
{
    int **mat1;
    int **mat2;
    int **result;
    int rows;
    int cols;
} thread_args;

typedef struct Vector Vector;
struct Vector
{
    int rows;
    int cols;
    double *elements;
};

typedef struct Matrix Matrix;
struct Matrix
{
    int rows;
    int cols;
    double **elements;
};

Vector *create_vector(int rows, int cols);
Vector *create_vector_from_file(const char *file_path, int rows, int cols);
Matrix *create_matrix(int rows, int cols);
Matrix *create_matrix_from_file(const char *file_path, int rows, int cols);
int **generate_random_matrix(int rows, int cols);

void init_matrix_rand(Matrix *M);
void init_vector_rand(Vector *V);
void copy_matrix(Matrix *dst, const Matrix *src);
void copy_vector(Vector *dst, const Vector *src);

void free_vector(Vector *v);
void free_matrix(Matrix *M);

Vector *add_vector(const Vector *a, const Vector *b);
//Vector *dot_vector_matrix(const Vector *v, const Matrix *M);
Matrix *add_matrix(const Matrix *M, const Matrix *N); //Punto 5
Matrix *dot_matrix(const Matrix *M, const Matrix *N); //Punto 6

Vector *matrix_col_mean(const Matrix *M);// Punto 1
Vector *matrix_col_sum(const Matrix *M);
Vector* matrix_col_max(const Matrix* M); //Punto 4
Vector* matrix_col_min(const Matrix* M); //Punto 4
Vector* matrix_col_vrz(const Matrix* M); //Punto 2
Vector* matrix_col_std(const Matrix* M); //Punto 3
void scalar_matrix(Matrix *M, double k); //Punto 7
void scalar_vector(Vector *V, double k);
void matrix_col_normalize_standard_score(Matrix *M); //Punto 8
void matrix_col_normalize_min_max(Matrix *M); //Punto 9

void print_vector(const Vector *v);
void print_matrix(const Matrix *M);

#endif
