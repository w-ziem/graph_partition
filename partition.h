#ifndef PARTITION_H
#define PARTITION_H

#include "graph.h"

// Struktura przechowująca podział grafu
typedef struct {
    int *partition; // Tablica przypisująca wierzchołek do podgrafu (0 lub 1)
    int numParts;   // Liczba części (tu: 2)
} partition_t;

// Funkcja obliczająca macierz Laplace'a
void compute_laplacian(graph_t *graph, double **laplacian);

// Funkcja znajdująca drugi najmniejszy wektor własny macierzy Laplace'a
double* compute_fiedler_vector(double **laplacian, int vertices);

// Funkcja dzieląca graf na dwie części na podstawie wektora Fiedlera
partition_t spectral_partition(graph_t *graph);

#endif // PARTITION_H
