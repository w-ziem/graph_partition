#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#define BUFSIZE 1024

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
    printf("Wczytano %d elemetów do verticies (2 linia)\n", count);
    

    //odczyt trzeciej linii
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości linii
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
    printf("Wczytano %d elemetów do edgeOffsets (3 linia)\n", count);
    


    //odczyt czwartej linii
    if(!fgets(buf, BUFSIZE, file)){
        handle_data_read_error(csrrg, "Błąd wczytywania linii");
    }

    //ustalanie długości linii
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
    printf("Wczytano %d elemetów do adjacency (4 linia)\n", count);



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
    printf("Wczytano %d elemetów do edgeIndices (5 linia)\n", count);

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
