#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/des.h>

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
    return strstr(temp, search) != NULL;
}

// Texto cifrado (puedes modificarlo con el que estás usando)
unsigned char cipher[] = {108, 245, 65, 63, 125, 200, 150, 66, 17, 170, 207, 170, 34, 31, 70, 215, 0};

int main() {
    long upper = (1L << 56); // Límite superior de claves DES (2^56)
    int ciphlen = strlen((char *)cipher);
    long found = 0;

    // Ciclo de fuerza bruta para probar cada clave
    for (long i = 0; i < upper; ++i) {
        if (tryKey(i, (char *)cipher, ciphlen)) {
            found = i;
            break; // Clave encontrada, salir del ciclo
        }

        // Mostrar progreso cada cierto número de intentos
        if (i % 1000000 == 0) {
            printf("Probando clave: %li\n", i);
        }
    }

    // Si se encontró la clave, desencriptar y mostrar el texto descifrado
    if (found != 0) {
        decrypt(found, (char *)cipher, ciphlen);
        printf("Clave encontrada: %li\nTexto descifrado: %s\n", found, cipher);
    } else {
        printf("No se encontró la clave en el rango especificado.\n");
    }

    return 0;
}
