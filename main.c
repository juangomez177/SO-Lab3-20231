#include <stdio.h>
#include "matrix.h"

int main()
{
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

    // Vector* result = dot_vector_matrix(v, M);
    // printf("Vector resultante: ");
    // print_vector(result);

    printf("Matrix: ");
    print_matrix(M);

    copy_matrix(M4,M);

    matrix_col_normalize_standard_score(M);

    printf("\nMatrix normalized: \n");
    print_matrix(M);

    printf("\nMatrix 4: \n");
    print_matrix(M4);
    matrix_col_normalize_min_max(M4);

    printf("\nMatrix 4 min max: \n");
    print_matrix(M4);

    free_matrix(M);
    free_matrix(M2);
    free_matrix(M3);
    free_matrix(M4);
    free_vector(v);
    free_vector(v2);
    // free_vector(result);

    return 0;
}
