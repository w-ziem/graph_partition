#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int vertices; //liczba wierzchołków
    int adjacencyCount; //...
    int *adjacency; //lista sąsiadów
    int *edgeIndices; //wskaźniki na początki grup sąsiadów
} graph_t;

typedef int** graph_matrix;

typedef struct {
    int maxVertices; //maksymalna możliwa liczba węzłów w wierszu (wielkość macierzy)
    int *vertices; //indeksy wierzchołków pogrupowane wierszami (w macierzy)
    int verticesCount;
    int *edgeOffsets; //wskaźniki, gdzie zaczyna się wiersz
    int edgeOffsetsCount;
    int *adjacency; //lista sąsiadów
    int adjacencyCount;
    int *edgeIndices; //lista wskaźników na grupy sąsiadów
    int edgeIndicesCount;
} csrrg_t;


// Funkcja do inicjalizacji pustego grafu
graph_t* create_graph(int verticesCount, int edgeIndicesCount, int adjacencyCount);

// Funkcja do wczytywania grafu z pliku w formacie CSRRG
graph_t* load_graph_from_csrrg(const csrrg_t* data);

// Funkcja do zwalniania pamięci zajmowanej przez graf
void free_graph(graph_t *graph);


// Funkcja wczytująca z danych wejściowych macierz odpowiadającą grafowi
graph_matrix load_matrix_from_csrrg(const csrrg_t* data);




#endif