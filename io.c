#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "error_handling.h"
#define BUFSIZE 99999

// Funkcja wczytująca graf z pliku
csrrg_t* parse_csrrg(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        handle_file_error(file, "Błąd otwarcia pliku");
    }

    csrrg_t *csrrg = (csrrg_t *)malloc(sizeof(csrrg_t));
    if (!csrrg) {
        handle_memory_error(csrrg, "Błąd alokacji pamięci dla CSRRG");
    }
    
    //odczytywanie pierwszej linii
    if(fscanf(file, "%d\n", &csrrg->maxVertices) != 1){
        handle_data_read_error(csrrg, "Bład wczytywania pierwszej linii CSRRG");
    }
    printf("CSRRG Structure:\n");
    printf("maxVertices: %d\n", csrrg->maxVertices);

    
    //odczyt drugiej linii
    char buf[BUFSIZE];
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości linii
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
    printf("Vertices Count: %d\n", csrrg->verticesCount);
    printf("Vertices:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", csrrg->vertices[i]);
    }
    printf("\n");
        
    //odczyt trzeciej linii
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości trzeciej linii
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
    
    printf("Edge Offsets:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", csrrg->edgeOffsets[i]);
    }
    printf("\n");
    
    //odczyt czwartej linii
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości czwartej linii
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
    printf("Adjacency Count: %d\n", csrrg->adjacencyCount);
    printf("Adjacency:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", csrrg->adjacency[i]);
    }
    printf("\n");

    //odczyt piątej linii
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości linii
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
    printf("Edge Indices count: %d\n", csrrg->edgeIndicesCount);
    printf("Edge Indices:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", csrrg->edgeIndices[i]);
    }
    printf("\n");


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
// Funkcja zapisująca podział binarnie
void save_partition_binary(const char *filename, partition_t partition, int vertices) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return;
    }

    uint8_t byte = 0;
    int bit_pos = 0;

    for (int i = 0; i < vertices; i++) {
        if (partition.partition[i]) {
            byte |= (1 << bit_pos);
        }
        bit_pos++;

        if (bit_pos == 8) {
            fwrite(&byte, sizeof(uint8_t), 1, file);
            byte = 0;
            bit_pos = 0;
        }
    }

    // Zapisz resztkę bajtu, jeśli wierzchołków nie było wielokrotnością 8
    if (bit_pos > 0) {
        fwrite(&byte, sizeof(uint8_t), 1, file);
    }

    fclose(file);
}


