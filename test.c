#include "graph.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>


void test_load_graph_from_csrrg(const char *filename) {
    csrrg_t *data = parse_csrrg(filename);
    if (data->adjacency == NULL || data->edgeIndices == NULL) {
        printf("Failed to parse CSRRG file: %s\n", filename);
        return;
    }

    graph_t *graph = load_graph_from_csrrg(data);
    if (graph == NULL) {
        printf("Failed to load graph from CSR representation.\n");
        free(data->adjacency);
        free(data->edgeIndices);
        return;
    }
    
    printf("Graph loaded with %d vertices.\n", graph->vertices);
    

}

void test_load_matrix_from_csrrg(const char *filename) {
    csrrg_t* data = parse_csrrg(filename);
    if (data->adjacency == NULL || data->edgeIndices == NULL) {
        printf("Failed to parse CSRRG file: %s\n", filename);
        return;
    }

    graph_matrix matrix = load_matrix_from_csrrg(data);
    if (matrix == NULL) {
        printf("Failed to load matrix from CSR representation.\n");
        free(data->adjacency);
        free(data->edgeIndices);
        return;
    }
    
    printf("Matrix loaded:\n");
    for (int i = 0; i < data->maxVertices; i++) {
        for (int j = 0; j < data->maxVertices; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < data->maxVertices; i++) {
        free(matrix[i]);
    }

}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s FILE.CSRRG\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    test_load_graph_from_csrrg(filename);
    test_load_matrix_from_csrrg(filename);
    
    return 0;
}