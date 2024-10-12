#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/des.h>
#include <mpi.h>

// Función para desencriptar con una clave dada
void decrypt(long key, char *ciph, int len) {
    DES_key_schedule schedule;
    DES_cblock des_key;

    // Convertir la clave a formato DES_cblock (8 bytes)
    for (int i = 0; i < 8; ++i) {
        des_key[i] = (key >> (56 - 8 * i)) & 0xFF;
    }

    // Configurar la clave y preparar el schedule
    DES_set_odd_parity(&des_key);
    DES_set_key_checked(&des_key, &schedule);

    // Desencriptar usando DES en modo ECB
    DES_ecb_encrypt((DES_cblock *)ciph, (DES_cblock *)ciph, &schedule, DES_DECRYPT);
}

// Función para verificar si el texto descifrado contiene la palabra clave
char search[] = " the ";
int tryKey(long key, char *ciph, int len) {
    char temp[len + 1];
    memcpy(temp, ciph, len);
    temp[len] = 0; // Asegurar que sea una cadena nula terminada
    decrypt(key, temp, len);
    //printf("Evaluando clave: %li -> Resultado: %s\n", key, strstr(temp, search) ? "Encontrada" : "No encontrada");
    return strstr(temp, search) != NULL;
}

// Estimar rangos probables (función ficticia para esta implementación)
void estimate_likely_ranges(long start_key, long end_key, long *ranges, int *num_ranges) {
    *num_ranges = 5; // Se aumentó el número de rangos
    for (int i = 0; i < *num_ranges; i++) {
        ranges[i] = start_key + i * (end_key - start_key) / (*num_ranges);
    }
    ranges[*num_ranges] = end_key;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Definir el texto cifrado
    char *cipher = "esta es una prueba"; // Sustituir por el texto cifrado real o cargar desde un archivo
    int ciphlen = strlen(cipher); // Ahora se inicializa correctamente

    long upper = (1L << 56); // Límite superior de claves DES (2^56)
    long ranges[6]; // Rangos estimados
    int num_ranges;
    
    if (rank == 0) {
        estimate_likely_ranges(0, upper, ranges, &num_ranges);
    }
    
    MPI_Bcast(ranges, 6, MPI_LONG, 0, MPI_COMM_WORLD);
    
    long found = 0;
    
    // Iterar sobre los rangos asignados a este proceso
    for (int i = rank; i < num_ranges - 1; i += size) {
        long start = ranges[i];
        long end = ranges[i + 1];

        for (long key = start; key < end; ++key) {
            if (tryKey(key, cipher, ciphlen)) { // Usar cipher aquí
                found = key;
                break;
            }
        }
    }
    
    // Si se encontró la clave, desencriptar y mostrar el texto descifrado
    if (found != 0) {
        decrypt(found, cipher, ciphlen);
        printf("Proceso %d: Clave encontrada: %li\nTexto descifrado: %s\n", rank, found, cipher);
    } else if (rank == 0) {
        printf("No se encontró la clave en el rango especificado.\n");
    }
    
    MPI_Finalize();
    return 0;
}
