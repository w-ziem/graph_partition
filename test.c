#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "partition.h"
#include "io.h"

// Funkcja testująca działanie programu
int main() {
    // Stałe ustawienia testu
    const char *input_file = "graf.csrrg";  // Ustaw nazwę swojego testowego pliku CSRG
    const char *output_base = "test_output";      // Baza nazwy plików wyjściowych
    int numParts = 2;                             // Liczba podziałów
    int margin = 10;                              // Margines procentowy
    int save_binary = 1;                          // Czy zapisać także plik binarny

    // Wczytanie grafu
    csrrg_t *graph_data = parse_csrrg(input_file);
    if (!graph_data) {
        fprintf(stderr, "Błąd wczytywania grafu!\n");
        return EXIT_FAILURE;
    }

    // Konwersja na strukturę grafu
    graph_t *graph = load_graph_from_csrrg(graph_data);
    if (!graph) {
        fprintf(stderr, "Błąd konwersji grafu!\n");
        return EXIT_FAILURE;
    }

    printf("Dzielenie grafu na %d części...\n", numParts);

    // Podział grafu
    partition_t partition = spectral_partition(graph, numParts);

    // Ewaluacja podziału
    evaluate_partition(graph, partition, margin);

    // Przygotowanie nazw plików wynikowych
    char output_txt[512];
    char output_bin[512];
    snprintf(output_txt, sizeof(output_txt), "%s.txt", output_base);
    snprintf(output_bin, sizeof(output_bin), "%s.bin", output_base);

    // Zapis wyników
    save_partition_to_file(output_txt, partition, graph->vertices);

    if (save_binary) {
        save_partition_binary(output_bin, partition, graph->vertices);
    }

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