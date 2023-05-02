# Solución de laboratorio de Paralelización en C
Este repositorio contiene la solución del laboratorio de Paralelización en C, en el que se pide paralelizar varias operaciones que se ejecutan durante el entrenamiento de redes neuronales. 
El programa principal se llama matrices y puede ejecutarse con varios flags para indicar la operación a realizar, el tamaño de los operandos y la ubicación de los archivos de entrada.

## Requisitos previos
Para compilar el código, se necesita instalar gcc.

## Compilar el programa
Para compilar el programa con gcc, se puede utilizar el siguiente comando:

```bash
gcc main.c matrix.c -o matrices
```

## Instrucciones de uso
Para ejecutar el programa con valores de operando aleatorios, utilice los siguientes flags:

```bash
./matrices -o [número de operación] -f [filas del primer operando] -c [columnas del primer operando] -r [filas del segundo operando] -s [columnas del segundo operando]
```

Para ejecutar el programa con los operandos de un archivo, utilice el siguiente flag:

```bash
./matrices -o [número de operación] -p [ruta del archivo de operandos]
```

## Operaciones disponibles
1. Calcular la media de cada columna de una matriz
2. Calcular la varianza de cada columna de una matriz
3. Calcular la desviación estándar de cada columna de una matriz
4. Calcular el valor mínimo y el valor máximo de cada columna de una matriz
5. Sumar dos matrices
6. Realizar el producto punto de dos matrices
7. Multiplicar un escalar por una matriz
8. Normalizar una matriz columna por columna con Standard Score
9. Normalizar una matriz columna por columna con Min-Max

## Créditos
Este laboratorio fue propuesto por el profesor Jheisson Argiro Lopez Restrepo en el laboratorio 3 de la clase de Sistemas Operativos. La solución en este repositorio fue escrita por Esneider Ballesta Paredes, Luis Felipe Cadavid Chica...
