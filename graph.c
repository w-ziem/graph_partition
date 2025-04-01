#include "graph.h"



int EDGES = 0;
graph_t *create_graph(int vertices, int edges) {
    graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
    if (!graph) {
        perror("Błąd alokacji pamięci dla grafu");
        return NULL;
    }
    graph->vertices = vertices;
    graph->adjacency = (int *)malloc(edges * sizeof(int));
    graph->edgeIndices = (int *)malloc((vertices + 1) * sizeof(int));
    
    if (!graph->adjacency || !graph->edgeIndices) {
        perror("Błąd alokacji pamięci dla struktury grafu");
        freeGraph(graph);
        return NULL;
    }
    return graph;
}


csrrg_t *parse_csrrg(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Błąd otwarcia pliku");
        return NULL;
    }


    csrrg_t *csrrg = (csrrg_t *)malloc(sizeof(csrrg_t));
    if (!csrrg) {
        perror("Błąd alokacji pamięci dla CSRRG");
        fclose(file);
        return NULL;
    }

    //wczytywanie pierwszego wiersza
    if (fscanf(file, "%d", &csrrg->maxVertices) != 1) {
        perror("Błąd odczytu maksymalnej liczby węzłów w wierszu");
        free(csrrg);
        fclose(file);
        return NULL;
    }

    //wczytywanie
    if (fscanf(file, "%d", &csrrg->vertices) != 1) {
        perror("Błąd odczytu liczby wierzchołków pogrupowanych wierszami");
        free(csrrg);
        fclose(file);
        return NULL;
    }

    csrrg->edgeOffsets = (int *)malloc((csrrg->maxVertices + 1) * sizeof(int));
    csrrg->adjacency = (int *)malloc(csrrg->maxVertices * sizeof(int));
    csrrg->edgeIndices = (int *)malloc((csrrg->maxVertices + 1) * sizeof(int));

    if (!csrrg->edgeOffsets || !csrrg->adjacency || !csrrg->edgeIndices) {
        perror("Błąd alokacji pamięci dla CSRRG");
        free(csrrg->edgeOffsets);
        free(csrrg->adjacency);
        free(csrrg->edgeIndices);
        free(csrrg);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i <= csrrg->maxVertices; i++) {
        if (fscanf(file, "%d", &csrrg->edgeOffsets[i]) != 1) {
            perror("Błąd odczytu edgeOffsets");
            free(csrrg->edgeOffsets);
            free(csrrg->adjacency);
            free(csrrg->edgeIndices);
            free(csrrg);
            fclose(file);
            return NULL;
        } else EDGES++;
    }

    for (int i = 0; i < csrrg->vertices; i++) {
        if (fscanf(file, "%d", &csrrg->adjacency[i]) != 1) {
            perror("Błąd odczytu adjacency");
            free(csrrg->edgeOffsets);
            free(csrrg->adjacency);
            free(csrrg->edgeIndices);
            free(csrrg);
            fclose(file);
            return NULL;
        }
    }

    for (int i = 0; i <= csrrg->vertices; i++) {
        if (fscanf(file, "%d", &csrrg->edgeIndices[i]) != 1) {
            perror("Błąd odczytu edgeIndices");
            free(csrrg->edgeOffsets);
            free(csrrg->adjacency);
            free(csrrg->edgeIndices);
            free(csrrg);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return csrrg;
}


graph_t* load_graph_from_csrrg(const csrrg_t data) {
    graph_t *graph = create_graph(data.maxVertices, EDGES);
    if (!graph) {
        return NULL;
    }
    graph->vertices = data.maxVertices;
    memcpy(graph->adjacency, data.adjacency, data.maxVertices* sizeof(int));
    memcpy(graph->edgeIndices, data.edgeIndices, (data.maxVertices + 1) * sizeof(int));
    return graph;
}


graph_matrix load_matrix_from_CSRRG(const csrrg_t data) {
    graph_matrix matrix;

    //alokowanie pamięci dal wierszy
    matrix = (int **)malloc(data.maxVertices * sizeof(int *));
    if (!matrix) {
        perror("Błąd alokacji pamięci dla macierzy grafu");
        return matrix;
    }


    //alokowanie pamięci dla kolumn
    for (int i = 0; i < data.maxVertices; i++) {
        matrix[i] = (int *)calloc(data.maxVertices, sizeof(int));
        if (!matrix[i]) {
            perror("Błąd alokacji pamięci dla wiersza macierzy grafu");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return matrix;
        }
    }

    // Wypełnienie macierzy zerami
    for (int i = 0; i < data.maxVertices; i++) {
        for (int j = 0; j < data.maxVertices; j++) {
            matrix[i][j] = 0;
        }
    }

    int col;
    for(int row = 0 ; row < data.maxVertices; row++){
        for(int i = data.edgeIndices[row]; i < data.edgeIndices[row++]; i++){
            col = data.vertices[i];
            matrix[row][col] = 1;
        }
    }

    return matrix;
}



void freeGraph(graph_t *graph) {
    if (graph) {
        free(graph->adjacency);
        free(graph->edgeIndices);
        free(graph);
    }
}


