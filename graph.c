#include "graph.h"
#include "error_handling.h"


graph_t *create_graph(int vertices, int edges) {
    graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
    if (!graph) {
        handle_memory_error(graph,"Błąd alokacji pamięci dla grafu");
        
    }
    graph->vertices = vertices;
    graph->adjacency = (int *)malloc(edges * sizeof(int));
    graph->edgeIndices = (int *)malloc((vertices + 1) * sizeof(int));
    
    if (!graph->adjacency || !graph->edgeIndices) {
        handle_memory_error(graph,"Błąd alokacji pamięci dla struktury grafu");
    }
    return graph;
}




graph_t* load_graph_from_csrrg(const csrrg_t* data) {
    int edges = data->edgeOffsets[data->maxVertices - 1];

    graph_t *graph = create_graph(data->maxVertices, edges);
    if (!graph) {
        return NULL;
    }
    graph->vertices = data->maxVertices;
    memcpy(graph->adjacency, data->adjacency, edges* sizeof(int));
    memcpy(graph->edgeIndices, data->edgeIndices, (data->maxVertices + 1) * sizeof(int));
    return graph;
}


graph_matrix load_matrix_from_csrrg(const csrrg_t* data) {
    graph_matrix matrix;

    //alokowanie pamięci dal wierszy
    matrix = (int **)malloc(data->maxVertices * sizeof(int *));
    if (!matrix) {
        handle_memory_error(matrix,"Błąd alokacji pamięci dla macierzy grafu");
    }


    //alokowanie pamięci dla kolumn
    for (int i = 0; i < data->maxVertices; i++) {
        matrix[i] = (int *)malloc(data->maxVertices * sizeof(int));
        if (!matrix[i]) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            handle_memory_error(matrix,"Błąd alokacji pamięci dla wiersza macierzy grafu");
        }
            
    }

    // Wypełnienie macierzy zerami
    for (int i = 0; i < data->maxVertices; i++) {
        for (int j = 0; j < data->maxVertices; j++) {
            matrix[i][j] = 0;
        }
    }


    // Wstawianie wierzchołków
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



void free_graph(graph_t *graph) {
    if (graph) {
        free(graph->adjacency);
        free(graph->edgeIndices);
        free(graph);
    }
}


