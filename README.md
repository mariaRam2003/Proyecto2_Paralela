# README

## Autores
María Marta Ramirez Gil 21342
Gustavo Andrés González Pineda 21438

## Descripción del Proyecto

Este proyecto contiene varios programas para realizar ataques de fuerza bruta y búsquedas paralelas/lineales para descifrar textos cifrados utilizando diferentes enfoques. Los algoritmos se ejecutan en un entorno distribuido y secuencial, apoyándose en tecnologías como MPI y OpenSSL.

### Archivos:

- **`bruteforce_sequential.c`**: Realiza un ataque de fuerza bruta de manera secuencial para descifrar un texto cifrado usando DES.
- **`bruteforce_mpi.c`**: Realiza un ataque de fuerza bruta utilizando MPI para ejecutar el proceso de forma distribuida.
- **`parteB.c`**: Enfocado en un ataque de fuerza bruta paralelo utilizando cifrado XOR para descifrar un texto en busca de una palabra clave específica.
- **`texto.txt`**: Archivo de texto que contiene el mensaje cifrado a descifrar.
- **`adaptive_search.c`**: Algoritmo que utiliza técnicas de búsqueda adaptativa para optimizar el ataque de fuerza bruta.
- **`binary_search.c`**: Implementación de búsqueda binaria en un entorno paralelo.

## Prerrequisitos

Antes de utilizar los programas, asegúrate de cumplir con los siguientes requisitos:

- **Docker**: Necesitarás tener Docker instalado en tu máquina para ejecutar el contenedor con el entorno preconfigurado.
- **OpenMPI**: Se utiliza para la ejecución paralela de los programas, incluido en el contenedor Docker.
- **OpenSSL**: Utilizado para la encriptación y desencriptación usando el algoritmo DES.

## Construcción del Entorno

Sigue estos pasos para construir el contenedor y compilar los programas.

1. **Clonar el repositorio** o copiar los archivos a tu máquina local:

```bash
git clone https://github.com/mariaRam2003/Proyecto2_Paralela.git
```

2. **Construir la imagen de Docker:**
Navega al directorio donde se encuentra el archivo Dockerfile y ejecuta el siguiente comando:

```bash
docker build -t bruteforce-app .
```
Este comando creará una imagen de Docker con todas las dependencias necesarias, compilará los programas y establecerá el entorno.

3. **Ejecutar el contenedor:**
Una vez creada la imagen, puedes ejecutar el contenedor usando el siguiente comando:

```bash
docker run -it bruteforce-app
```
Este comando iniciará el contenedor y ejecutará por defecto el programa bruteforce_mpi usando 4 procesos.

### Archivos de Entrada:
El archivo de texto texto.txt contiene el mensaje cifrado que los programas intentan descifrar. Asegúrate de modificar el contenido según el mensaje cifrado que quieras usar en tus pruebas.

## Uso de los Programas
Ejecución Secuencial
Para ejecutar el programa de fuerza bruta secuencial bruteforce_sequential:

``` bash
./bruteforce_sequential
```
Este programa intentará descifrar el texto cifrado utilizando un ataque de fuerza bruta secuencialmente.

## Ejecución Paralela con MPI
Para ejecutar el programa bruteforce_mpi en paralelo utilizando MPI:

``` bash
mpirun -np <número_de_procesos> ./bruteforce_mpi
```

Por ejemplo, para ejecutarlo con 4 procesos:

``` bash
mpirun -np 4 ./bruteforce_mpi
```

## Ejecución del Programa ParteB
Este programa también utiliza MPI y requiere un parámetro de clave inicial. Para ejecutarlo, puedes usar el siguiente comando:

``` bash
mpirun -np <número_de_procesos> ./parteB <clave_inicial>
```

Por ejemplo:

``` bash
mpirun -np 4 ./parteB 123456789
```

## Ejecución de los Algoritmos de Búsqueda

Para ejecutar el programa de búsqueda adaptativa adaptive_search:

```bash
mpirun -np <número_de_procesos> ./adaptive_search
```

Para ejecutar el programa de búsqueda binaria binary_search:
``` bash
mpirun -np <número_de_procesos> ./binary_search
``` 

## Entrar al contenedor:
``` bash
docker run -it bruteforce-app /bin/bash
``` 
Realizar los cambios en los archivos de código que desees modificar.

### Compilar nuevamente el programa modificado. Por ejemplo, si modificaste bruteforce_sequential.c:
```bash
gcc -o bruteforce_sequential bruteforce_sequential.c -lssl -lcrypto
```
### Ejecutar el programa una vez compilado:
```bash
./bruteforce_sequential
```
