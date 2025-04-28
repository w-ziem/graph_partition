#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "graph.h"
#include "partition.h"
#include "io.h"

int main(int argc, char *argv[]) {
    // Zmienne konfiguracyjne programu
    char *input_file = NULL;
    char *output_base = "wynik_podzialu";  // Domyślna nazwa pliku wyjściowego (bez rozszerzenia)
    int numParts = 2;                      // Domyślna liczba podgrafów
    int margin = 10;                       // Domyślny margines procentowy
    int save_binary = 0;                   // Flaga zapisu pliku binarnego

    // Parsowanie argumentów linii poleceń
    int opt;
    while ((opt = getopt(argc, argv, "i:p:m:o:b")) != -1) {
        switch (opt) {
            case 'i':  // Plik wejściowy
                input_file = optarg;
                break;
            case 'p':  // Liczba podgrafów
                numParts = atoi(optarg);
                if (numParts <= 0) {
                    fprintf(stderr, "Liczba podgrafów musi być dodatnia\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'm':  // Margines procentowy
                margin = atoi(optarg);
                if (margin < 0 || margin > 100) {
                    fprintf(stderr, "Margines musi być w zakresie 0-100\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'o':  // Baza nazwy plików wyjściowych
                output_base = optarg;
                break;
            case 'b':  // Włączenie zapisu binarnego
                save_binary = 1;
                break;
            default:  // Nieprawidłowa opcja
                fprintf(stderr, "Użycie: %s -i <plik_wejsciowy> [-p liczba_podgrafow] [-m margines] [-o baza_wyjscia] [-b]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Sprawdzenie obowiązkowego pliku wejściowego
    if (!input_file) {
        fprintf(stderr, "Plik wejściowy jest wymagany (-i <plik>)\n");
        return EXIT_FAILURE;
    }

    // Wczytanie grafu z pliku w formacie CSRRG
    csrrg_t *graph_data = parse_csrrg(input_file);
    if (!graph_data) {
        fprintf(stderr, "Błąd wczytywania grafu!\n");
        return EXIT_FAILURE;
    }

    // Konwersja danych CSRRG na strukturę grafu
    graph_t *graph = load_graph_from_csrrg(graph_data);
    if (!graph) {
        fprintf(stderr, "Błąd konwersji grafu!\n");
        return EXIT_FAILURE;
    }

    printf("\nDzielenie grafu na %d części...\n", numParts);

    // Podział grafu metodą spektralną
    partition_t partition = spectral_partition(graph, numParts);

    // Ewaluacja jakości podziału (np. margines różnicy liczby wierzchołków)
    evaluate_partition(graph, partition, margin);

    // Przygotowanie nazw plików wynikowych
    char output_txt[512];
    char output_bin[512];
    snprintf(output_txt, sizeof(output_txt), "%s.txt", output_base);
    snprintf(output_bin, sizeof(output_bin), "%s.bin", output_base);

    // Zapis wyniku podziału do pliku tekstowego
    save_partition_to_file(output_txt, partition, graph->vertices);

    // Opcjonalny zapis wyniku także do pliku binarnego
    if (save_binary) {
        save_partition_binary(output_bin, partition, graph->vertices);
    }

    // Zwolnienie zaalokowanej pamięci
    free(partition.partition);
    free_graph(graph);
    free(graph_data->vertices);
    free(graph_data->edgeOffsets);
    free(graph_data->adjacency);
    free(graph_data->edgeIndices);
    free(graph_data);

    return EXIT_SUCCESS;
}