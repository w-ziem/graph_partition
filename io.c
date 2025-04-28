#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "error_handling.h"
#define BUFSIZE 99999  // Rozmiar bufora do wczytywania linii z pliku

// Funkcja wczytująca graf z pliku w formacie CSRRG
csrrg_t* parse_csrrg(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        handle_file_error(file, "Błąd otwarcia pliku");
    }

    // Alokacja struktury na dane grafu
    csrrg_t *csrrg = (csrrg_t *)malloc(sizeof(csrrg_t));
    if (!csrrg) {
        handle_memory_error(csrrg, "Błąd alokacji pamięci dla CSRRG");
    }
    
    // Wczytanie liczby wierzchołków
    if (fscanf(file, "%d\n", &csrrg->maxVertices) != 1) {
        handle_data_read_error(csrrg, "Bład wczytywania pierwszej linii CSRRG");
    }
    
    char buf[BUFSIZE];
    
    // Wczytanie listy sąsiadów (vertices)
    if (!fgets(buf, BUFSIZE, file)) {
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    int count = 1;
    for (char *p = buf; *p; p++) {
        if (*p == ';') count++;
    }
    csrrg->vertices = malloc(count * sizeof(int));
    char *token = strtok(buf, ";\n");
    int j = 0;
    while (token) {
        csrrg->vertices[j++] = atoi(token);
        token = strtok(NULL, ";\n");
    }
    csrrg->verticesCount = count;


    // Wczytanie tablicy offsetów krawędzi (edgeOffsets)
    if (!fgets(buf, BUFSIZE, file)) {
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    count = 1;
    for (char *p = buf; *p; p++) {
        if (*p == ';') count++;
    }
    csrrg->edgeOffsets = malloc(count * sizeof(int));
    token = strtok(buf, ";\n");
    j = 0;
    while (token) {
        csrrg->edgeOffsets[j++] = atoi(token);
        token = strtok(NULL, ";\n");
    }
    csrrg->edgeOffsetsCount = count;


    // Wczytanie tablicy sąsiadów (adjacency list)
    if (!fgets(buf, BUFSIZE, file)) {
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    count = 1;
    for (char *p = buf; *p; p++) {
        if (*p == ';') count++;
    }
    csrrg->adjacency = malloc(count * sizeof(int));
    token = strtok(buf, ";\n");
    j = 0;
    while (token) {
        csrrg->adjacency[j++] = atoi(token);
        token = strtok(NULL, ";\n");
    }
    csrrg->adjacencyCount = count;

    // Wczytanie indeksów krawędzi (edgeIndices)
    if (!fgets(buf, BUFSIZE, file)) {
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    count = 1;
    for (char *p = buf; *p; p++) {
        if (*p == ';') count++;
    }
    csrrg->edgeIndices = malloc(count * sizeof(int));
    token = strtok(buf, ";\n");
    j = 0;
    while (token) {
        csrrg->edgeIndices[j++] = atoi(token);
        token = strtok(NULL, ";\n");
    }
    csrrg->edgeIndicesCount = count;


    fclose(file);
    return csrrg;
}

// Funkcja zapisująca wyniki podziału grafu do pliku tekstowego
void save_partition_to_file(const char *filename, partition_t partition, int vertices) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        handle_file_error(file, "Błąd otwarcia pliku do zapisu");
        return;
    }

    // Zapis: numer wierzchołka -> numer podgrafu
    for (int i = 0; i < vertices; i++) {
        fprintf(file, "Wierzchołek %d -> Podgraf %d\n", i, partition.partition[i]);
    }

    fclose(file);
}

// Funkcja zapisująca wyniki podziału grafu do pliku binarnego
void save_partition_binary(const char *filename, partition_t partition, int vertices) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return;
    }

    uint8_t byte = 0;
    int bit_pos = 0;

    // Wypełnianie bajtów bitami zgodnie z przynależnością do podgrafu
    for (int i = 0; i < vertices; i++) {
        if (partition.partition[i]) {
            byte |= (1 << bit_pos);
        }
        bit_pos++;

        // Zapisz pełny bajt do pliku
        if (bit_pos == 8) {
            fwrite(&byte, sizeof(uint8_t), 1, file);
            byte = 0;
            bit_pos = 0;
        }
    }

    // Zapisz pozostałe bity jeśli coś zostało
    if (bit_pos > 0) {
        fwrite(&byte, sizeof(uint8_t), 1, file);
    }

    fclose(file);
}