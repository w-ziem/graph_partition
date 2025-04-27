#include "graph.h"
#include "error_handling.h"


int EDGES = 0;
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
    graph_t *graph = create_graph(data->maxVertices, EDGES);
    if (!graph) {
        return NULL;
    }
    graph->vertices = data->maxVertices;
    memcpy(graph->adjacency, data->adjacency, data->maxVertices* sizeof(int));
    memcpy(graph->edgeIndices, data->edgeIndices, (data->maxVertices + 1) * sizeof(int));
    return graph;
}


graph_matrix load_matrix_from_csrrg(const csrrg_t* data) {
    graph_matrix matrix;
    if (!data->edgeOffsets) {
        fprintf(stderr, "edgeOffsets to NULL\n");
        exit(EXIT_FAILURE);
    }

    //alokowanie pamięci dal wierszy
    matrix = (int **)malloc(data->maxVertices * sizeof(int *));
    printf("Zaalokowalem pamiec dla wierszy!! \n");
    if (!matrix) {
        handle_memory_error(matrix,"Błąd alokacji pamięci dla macierzy grafu");
    }


    //alokowanie pamięci dla kolumn
    printf("Zaczynam alokować pamięć dla kolumn... \n");
    for (int i = 0; i < data->maxVertices; i++) {
        matrix[i] = (int *)malloc(data->maxVertices * sizeof(int));
        printf("Zaalokowalem: %d kolumne\n",i);
        if (!matrix[i]) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            handle_memory_error(matrix,"Błąd alokacji pamięci dla wiersza macierzy grafu");
        }
            
    }
    printf("Zaalokowalem pamiec dla kolumn!! \n");
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
        if (row + 1 >= data->maxVertices + 1) {
            fprintf(stderr, "Błąd: row+1 poza zakresem edgeOffsets (row=%d)\n", row);
            break;
        }
        int end;
        if (row + 1 >= data->maxVertices + 1) {
            fprintf(stderr, "Błąd: edgeOffsets[%d] poza zakresem (max=%d)\n", row + 1, data->maxVertices);
            end = data->edgeOffsets[row];  // zapobiega wyjściu poza zakres
        } else {
            end = data->edgeOffsets[row + 1];
        }
    
        for (int i = start; i < end; i++) {
            if(col >= data->maxVertices) {
                fprintf(stderr, "Błąd: col=%d przekracza maxVertices=%d\n", col, data->maxVertices);
                continue;
            }
            col = data->vertices[i];
            printf("Obecny wiersz: %d , kolumna: %d \n",row,col);
            if (col < 0 || col >= data->maxVertices) {
                fprintf(stderr, "Błąd: niepoprawny indeks col=%d przy row=%d (max=%d)\n", col, row, data->maxVertices - 1);
            } else {
                matrix[row][col] = 1;
            }
        }
        
        printf("Row %d: start=%d, end=%d\n", row, start, end);
        for (int i = start; i < end; i++) {
            printf("  Setting matrix[%d][%d] = 1\n", row, data->vertices[i]);
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


