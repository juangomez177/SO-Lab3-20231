#ifndef MATRIX_H
#define MATRIX_H

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
    int thread_id;    // id del hilo
    int thread_count; // Cantidad total de hilos
    double escalar;   // Escalar para punto 7
    const Matrix *M;  // Puntero a Matriz 1
    const Matrix *N;  // Puntero a Matriz 2
    Matrix *result;   // Puntero a Matriz Resultante
    Vector *V;        // Puntero a Vector Resultante, cuando se realizan operaciones a una sóla matriz
};

// Creacion de vectores
Vector *create_vector(int rows, int cols);
// Vector *create_vector_from_file(const char *file_path, int rows, int cols);

// Cración de matrices
Matrix *create_matrix(int rows, int cols);
// Matrix *create_matrix_from_file(const char *file_path, int rows, int cols);

// Llenado random de vectores y matrices
void init_vector_rand(Vector *V);
void init_matrix_rand(Matrix *M);

// Copiado de un vector y una matriz
// void copy_vector(Vector *dst, const Vector *src);
void copy_matrix(Matrix *dst, const Matrix *src);

// Liberación de un vector o una matriz
void free_vector(Vector *v);
void free_matrix(Matrix *M);

// Utilidades
// Vector *add_vector(const Vector *a, const Vector *b);
// void scalar_vector(Vector *V, double k);
void print_vector(const Vector *v);
void print_matrix(const Matrix *M);

// Metodos paralelizados (puntos 1 al 9)
void *Matrix_col_mean(void *thread_arg);                      // Punto 1 mean
void *Matrix_col_vrz(void *thread_arg);                       // Punto 2 vrz
void *Matrix_col_sum(void *thread_arg);                       // Punto 2 Suma individual de columnas
void *Matrix_col_std(void *thread_arg);                       // Punto 3 std
double sqrt(double number);                                   // Punto 3 para el calculo de sqrt
void *Matrix_col_max(void *thread_arg);                       // Punto 4 max
void *Matrix_col_min(void *thread_arg);                       // Punto 4 min
void *Add_matrix(void *thread_arg);                           // Punto 5 Suma de matrices
void *Dot_matrix(void *thread_arg);                           // Punto 6 Producto punto
void *Scalar_matrix(void *thread_arg);                        // Punto 7 Escalar
void *Matrix_col_normalized_standard_score(void *thread_arg); // Punto 8 Nomalize
void *Matrix_col_normalized_min_max(void *thread_arg);        // Punto 9 Nomalize

// Metodos para la creacion de los hilos y el llamado a las distintas operaciones
Vector *init_vector_threads(const Matrix *M, int thread_count, int operation);                  // Metodo que maneja los hilos para puntos 1, 2, 3 y 4. Genera un vector como resultado
Matrix *init_matrix_threads(const Matrix *M, const Matrix *N, int thread_count, int operation); // Metodo que maneja los hilos para puntos 5 y 6. Genera una nueva matriz como resultado
Matrix *init_matrix_threads_void(Matrix *M, int thread_count, int operation, double k);         // Metodo que maneja los hilos para puntos 7, 8 y 9. Modifican la matriz original

#endif
