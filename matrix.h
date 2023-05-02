#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

// Estructura para los argumentos que se pasarán a los hilos

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

typedef struct Thread_data Thread_data;
struct Thread_data
{
    int thread_id;
    int thread_count;
    double k;
    const Matrix *M;
    const Matrix *N;
    Matrix *result;
    Vector *V;
};

//Creacion y liberacion de vectores y matrices
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
//---------------------------------------------

//Metodos no paralelizados que aparentemente no son necesarios    
Vector *add_vector(const Vector *a, const Vector *b);
void scalar_vector(Vector *V, double k);
void print_vector(const Vector *v);
void print_matrix(const Matrix *M);
//---------------------------------------------

// Metodos paralelizados del laboratorio (puntos 1 al 9)
// Probar todos los metodos de manera exhaustiva para buscar errores
double sqrt(double number);
void *Matrix_col_mean(void *thread_arg);// Punto 1
void *Matrix_col_vrz(void *thread_arg);// Punto 2
void *Matrix_col_std(void *thread_arg);// Punto 3
void *Matrix_col_max(void *thread_arg);// Punto 4
void *Matrix_col_min(void *thread_arg);// Punto 4
void *Matrix_col_sum(void *thread_arg);
void *Add_matrix(void *thread_arg);// Punto 5
void *Dot_matrix(void *thread_arg);// Punto 6
void *Scalar_matrix(void *thread_arg);// Punto 7
void *Matrix_col_normalized_standard_score(void *thread_arg);// Punto 8
void *Matrix_col_normalized_min_max(void *thread_arg);// Punto 9
//---------------------------------------------

//Metodos para la creacion de los hilos y el llamado de los metodos del taller
Vector *init_vector_threads(const Matrix *M, int thread_count, int operation);
Matrix *init_matrix_threads(const Matrix *M, const Matrix *N, int thread_count, int operation);
void *init_matrix_threads_void(const Matrix *M, int thread_count, int operation, double k);
//---------------------------------------------

#endif
