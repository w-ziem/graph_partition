#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include "error_handling.h"

// Funkcja wczytująca graf z pliku
csrrg_t* parse_csrrg(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
       handle_file_error(file,"Błąd otwarcia pliku");
    }

    csrrg_t *csrrg = (csrrg_t *)malloc(sizeof(csrrg_t));
    if (!csrrg) {
        handle_memory_error(csrrg,"Błąd alokacji pamięci dla CSRRG");
    }

    if (fscanf(file, "%d", &csrrg->maxVertices) != 1) {
        handle_data_read_error(csrrg, "Błąd odczytu maksymalnej liczby węzłów");
    }

    if (fscanf(file, "%d", &csrrg->vertices) != 1) {
        handle_data_read_error(csrrg,"Błąd odczytu liczby wierzchołków");
    }

    csrrg->edgeOffsets = (int *)malloc((csrrg->maxVertices + 1) * sizeof(int));
    csrrg->adjacency = (int *)malloc(csrrg->maxVertices * sizeof(int));
    csrrg->edgeIndices = (int *)malloc((csrrg->maxVertices + 1) * sizeof(int));

    if (!csrrg->edgeOffsets || !csrrg->adjacency || !csrrg->edgeIndices) {
        handle_memory_error(csrrg,"Błąd alokacji pamięci dla CSRRG");
    }

    for (int i = 0; i <= csrrg->maxVertices; i++) {
        if (fscanf(file, "%d", &csrrg->edgeOffsets[i]) != 1) {
            handle_data_read_error(csrrg, "Błąd odczytu edgeOffsets");
        }
    }

    for (int i = 0; i < csrrg->vertices; i++) {
        if (fscanf(file, "%d", &csrrg->adjacency[i]) != 1) {
            handle_data_read_error(csrrg,"Błąd odczytu adjacency");
        }
    }

    for (int i = 0; i <= csrrg->vertices; i++) {
        if (fscanf(file, "%d", &csrrg->edgeIndices[i]) != 1) {
            handle_data_read_error(csrrg,"Błąd odczytu edgeIndices");
        }
    }

    fclose(file);
    return csrrg;
}

// Funkcja zapisująca wyniki podziału grafu do pliku
void save_partition_to_file(const char *filename, partition_t partition, int vertices) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        handle_file_error(file,"Błąd otwarcia pliku do zapisu");
        return;
    }

    for (int i = 0; i < vertices; i++) {
        fprintf(file, "Wierzchołek %d -> Podgraf %d\n", i, partition.partition[i]);
    }

    fclose(file);
}
