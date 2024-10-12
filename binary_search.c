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
    temp[len] = 0; // Asegurar que sea una cadena nula terminadas
    decrypt(key, temp, len);
    // printf("Evaluando clave: %li -> Resultado: %s\n", key, strstr(temp, search) ? "Encontrada" : "No encontrada");
    return strstr(temp, search) != NULL;
}

unsigned char cipher[] = {108, 245, 65, 63, 125, 200, 150, 66, 17, 170, 207, 170, 34, 31, 70, 215, 0};

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    long upper = (1L << 56); // Límite superior de claves DES (2^56)
    long found = 0;
    
    long start_key = 0;
    long end_key = upper;
    
    while (start_key < end_key) {
        long mid_key = (start_key + end_key) / 2;

        // Cada proceso buscará en su rango
        for (long key = start_key; key <= mid_key; ++key) {
            if (tryKey(key, (char *)cipher, strlen((char *)cipher))) {
                found = key;
                break;
            }
        }

        if (found != 0) {
            // Si se encuentra la clave, notificar a otros procesos
            for (int i = 0; i < size; i++) {
                if (i != rank) {
                    MPI_Send(&found, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
                }
            }
            break;
        }

        // Proceso maestro espera el resultado de los otros procesos
        MPI_Bcast(&found, 1, MPI_LONG, 0, MPI_COMM_WORLD);

        // Determinar el nuevo rango basado en la búsqueda
        if (found == 0) {
            // Si no se encontró la clave, seguir buscando en la otra mitad
            start_key = mid_key + 1; // Reemplazar el rango de inicio
        } else {
            end_key = mid_key; // Si se encontró, continuar buscando en la mitad izquierda
        }
    }
    
    // Si se encontró la clave, desencriptar y mostrar el texto descifrado
    if (found != 0) {
        decrypt(found, (char *)cipher, strlen((char *)cipher));
        printf("Proceso %d: Clave encontrada: %li\nTexto descifrado: %s\n", rank, found, cipher);
    } else if (rank == 0) {
        printf("No se encontró la clave en el rango especificado.\n");
    }
    
    MPI_Finalize();
    return 0;
}
