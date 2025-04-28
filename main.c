#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // dla getopt
#include "graph.h"
#include "partition.h"
#include "io.h"

int main(int argc, char *argv[]) {
    int opt;
    char *graph_filename = NULL;
    int numParts = 2; // Domyślna liczba podgrafów

    // Parsowanie opcji
    while ((opt = getopt(argc, argv, "f:p:")) != -1) {
        switch (opt) {
            case 'f':
                graph_filename = optarg;
                break;
            case 'p':
                numParts = atoi(optarg);
                if (numParts <= 0) {
                    fprintf(stderr, "Liczba podgrafów musi być dodatnia\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                fprintf(stderr, "Użycie: %s -f <plik_grafu> [-p liczba_podgrafów]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (graph_filename == NULL) {
        fprintf(stderr, "Brak nazwy pliku z grafem!\n");
        fprintf(stderr, "Użycie: %s -f <plik_grafu> [-p liczba_podgrafów]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Wczytanie grafu z pliku
    csrrg_t *graph_data = parse_csrrg(graph_filename);
    if (!graph_data) {
        fprintf(stderr, "Błąd wczytywania grafu!\n");
        return EXIT_FAILURE;
    }

    // Konwersja CSRRG na wewnętrzną strukturę grafu
    graph_t *graph = load_graph_from_csrrg(graph_data);
    if (!graph) {
        fprintf(stderr, "Błąd konwersji grafu!\n");
        return EXIT_FAILURE;
    }

    printf("Dzielenie grafu na %d części...\n", numParts);

    // Podział grafu metodą spektralną z określoną liczbą części
    partition_t partition = spectral_partition(graph, numParts);
    evaluate_partition(graph, partition, 10);

    // Zapisanie wyniku podziału do pliku
    save_partition_to_file("wynik_podzialu.txt", partition, graph->vertices);

    // Zwolnienie pamięci
    free(partition.partition);
    free_graph(graph);
    free(graph_data->vertices);
    free(graph_data->edgeOffsets);
    free(graph_data->adjacency);
    free(graph_data->edgeIndices);
    free(graph_data);

    return EXIT_SUCCESS;
}