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

    printf("Generated graph\n");
    printf("vertices: %d\n", graph->vertices);

    printf("adjacency: ");
    for (int i = 0; i < 248; i++) {
        printf("%d", data->adjacency[i]);
        if (i < 248) {
            printf(", ");
        }
    }
    printf("\n");
    
    printf("\n");
    printf("%ld\n", sizeof(data->edgeIndices));
    printf("\n");
    printf("edgeIndices: ");
    for (int i = 0; i < 72; i++) { // Assuming edgeIndices size is maxVertices + 1
        printf("%d", data->edgeIndices[i]);
        if (i < 72) {
            printf(", ");
        }
    }
    printf("\n");

    free(data->adjacency);
    free(data->edgeIndices);
}

void test_load_matrix_from_csrrg(const char *Filename) {
    printf("Rozpoczynam wczytywanie macierzy z csrrg \n");
    csrrg_t* data1 = parse_csrrg(Filename);
    if (data1->adjacency == NULL || data1->edgeIndices == NULL) {
        printf("Failed to parse CSRRG file: %s\n", Filename);
        return;
    }
    printf("Parse csrrg sie udalo \n");
    graph_matrix matrix = load_matrix_from_csrrg(data1);
    if (matrix == NULL) {
        printf("Failed to load matrix from CSR representation.\n");
        free(data1->adjacency);
        free(data1->edgeIndices);
        return;
    }
    
    printf("Matrix loaded:\n");
    for (int i = 0; i < data1->maxVertices; i++) {
        for (int j = 0; j < data1->maxVertices; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < data1->maxVertices; i++) {
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