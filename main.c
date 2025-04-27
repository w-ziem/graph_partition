#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "partition.h"
#include "io.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <plik_grafu>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Wczytanie grafu z pliku
    csrrg_t *graph_data = parse_csrrg(argv[1]);
    if (!graph_data) {
        fprintf(stderr, "Błąd wczytywania grafu!\n");
        return EXIT_FAILURE;
    }
    printf("wczytano z pliku!!");
    // Konwersja CSRRG na wewnętrzną strukturę grafu
    graph_t *graph = load_graph_from_csrrg(graph_data);
    if (!graph) {
        fprintf(stderr, "Błąd konwersji grafu!\n");
        return EXIT_FAILURE;
    }

    // Podział grafu metodą spektralną
    partition_t partition = spectral_partition(graph);

    // Zapisanie wyniku podziału do pliku
    save_partition_to_file("wynik_podzialu.txt", partition, graph->vertices);

    // Zwolnienie pamięci
    free(partition.partition);
    free_graph(graph);
    free(graph_data->edgeOffsets);
    free(graph_data->adjacency);
    free(graph_data->edgeIndices);
    free(graph_data);

    return EXIT_SUCCESS;
}
