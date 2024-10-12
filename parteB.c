#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void encrypt(char *input, char *output, unsigned long long key) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        output[i] = input[i] ^ ((key >> (8 * (i % 8))) & 0xFF);
    }
    output[len] = '\0';
}

int contains_keyword(char *text, const char *keyword) {
    return strstr(text, keyword) != NULL;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *keyword = "es una prueba de";
    char text[256], encrypted[256], decrypted[256];

    FILE *file = fopen("texto.txt", "r");
    if (!file) {
        perror("Error al abrir el archivo");
        MPI_Finalize();
        return 1;
    }
    fgets(text, sizeof(text), file);
    fclose(file);

    if (argc != 2) {
        if (rank == 0) {
            printf("Uso: %s <clave inicial>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    unsigned long long start_key = strtoull(argv[1], NULL, 10);
    unsigned long long range = 1000000000ULL;  // 1 billion
    unsigned long long range_per_process = range / size;

    unsigned long long local_start_key = start_key + rank * range_per_process;
    unsigned long long local_end_key = (rank == size - 1) ? start_key + range : local_start_key + range_per_process;

    // Cifrado inicial con la clave proporcionada
    if (rank == 0) {
        encrypt(text, encrypted, start_key);
        printf("Texto cifrado: %s\n", encrypted);
    }

    MPI_Bcast(encrypted, 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    int global_found = 0;
    unsigned long long found_key = 0;

    for (unsigned long long key = local_start_key; key < local_end_key && !global_found; key++) {
        encrypt(encrypted, decrypted, key);

        if (contains_keyword(decrypted, keyword)) {
            printf("Proceso %d encontró una posible clave: %llu\n", rank, key);
            printf("Texto descifrado: %s\n", decrypted);
            
            // Verificar si es la clave correcta
            char re_encrypted[256];
            encrypt(text, re_encrypted, key);
            if (strcmp(re_encrypted, encrypted) == 0) {
                found_key = key;
                global_found = 1;
                printf("Proceso %d confirmó la clave correcta: %llu\n", rank, key);
            }
        }

        if (key % 10000000 == 0) {  // Imprimir progreso cada 10 millones de intentos
            printf("Proceso %d: Probando clave %llu\n", rank, key);
        }

        MPI_Allreduce(MPI_IN_PLACE, &global_found, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        if (global_found) {
            printf("La clave fue encontrada: %llu\n", found_key);
        } else {
            printf("No se encontró la clave en el rango especificado.\n");
        }
        printf("Tiempo total de ejecución: %f segundos\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}