#include "graph.h"
#include "error_handling.h"

// Funkcja tworząca nową strukturę grafu
graph_t *create_graph(int verticesCount, int edgeIndicesCount, int adjacencyCount) {
    graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
    if (!graph) {
        handle_memory_error(graph, "Błąd alokacji pamięci dla grafu");
    }

    graph->vertices = verticesCount;
    graph->adjacency = (int *)malloc(adjacencyCount * sizeof(int));
    graph->edgeIndices = (int *)malloc(edgeIndicesCount * sizeof(int));
    
    if (!graph->adjacency || !graph->edgeIndices) {
        handle_memory_error(graph, "Błąd alokacji pamięci dla struktury grafu");
    }
    return graph;
}

// Funkcja ładująca graf z danych CSRRG do struktury graph_t
graph_t* load_graph_from_csrrg(const csrrg_t* data) {
    graph_t *graph = create_graph(data->verticesCount, data->edgeIndicesCount, data->adjacencyCount);
    if (!graph) {
        return NULL;
    }

    graph->vertices = data->verticesCount;
    graph->adjacencyCount = data->adjacencyCount;

    // Kopiowanie danych sąsiedztwa i indeksów krawędzi
    memcpy(graph->adjacency, data->adjacency, data->adjacencyCount * sizeof(int));
    memcpy(graph->edgeIndices, data->edgeIndices, data->edgeIndicesCount * sizeof(int));

    return graph;
}

// Funkcja konwertująca dane CSRRG na pełną macierz sąsiedztwa
graph_matrix load_matrix_from_csrrg(const csrrg_t* data) {
    graph_matrix matrix;

    // Alokacja pamięci na tablicę wskaźników (wierszy)
    matrix = (int **)malloc(data->maxVertices * sizeof(int *));
    if (!matrix) {
        handle_memory_error(matrix, "Błąd alokacji pamięci dla macierzy grafu");
    }

    // Alokacja pamięci na każdy wiersz (kolumny)
    for (int i = 0; i < data->maxVertices; i++) {
        matrix[i] = (int *)malloc(data->maxVertices * sizeof(int));
        if (!matrix[i]) {
            // W przypadku błędu pamięci zwalniamy wszystko co do tej pory zaalokowano
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            handle_memory_error(matrix, "Błąd alokacji pamięci dla wiersza macierzy grafu");
        }
    }

    // Inicjalizacja macierzy zerami
    for (int i = 0; i < data->maxVertices; i++) {
        for (int j = 0; j < data->maxVertices; j++) {
            matrix[i][j] = 0;
        }
    }

    // Wstawienie jedynek tam, gdzie istnieją krawędzie
    int col;
    for (int row = 0; row < data->maxVertices; row++) {
        int start = data->edgeOffsets[row];
        int end = data->edgeOffsets[row + 1];
    
        for (int i = start; i < end; i++) {
            col = data->vertices[i];
            matrix[row][col] = 1;
        }
    }

    return matrix;
}

// Funkcja zwalniająca pamięć zajmowaną przez graf
void free_graph(graph_t *graph) {
    if (graph) {
        free(graph->adjacency);
        free(graph->edgeIndices);
        free(graph);
    }
}