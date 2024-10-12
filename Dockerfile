# Usar una imagen base de Ubuntu
FROM ubuntu:latest

# Instalar dependencias necesarias
RUN apt-get update && \
    apt-get install -y build-essential openmpi-bin libopenmpi-dev libssl-dev && \
    apt-get clean

# Establecer directorio de trabajo
WORKDIR /usr/src/app

# Copiar los archivos fuente al contenedor
COPY bruteforce_mpi.c . 
COPY bruteforce_sequential.c . 
COPY parteB.c . 
COPY texto.txt . 
COPY adaptive_search.c . 
COPY binary_search.c . 

# Compilar los programas
RUN mpicc -o bruteforce_mpi bruteforce_mpi.c -lssl -lcrypto
RUN gcc -o bruteforce_sequential bruteforce_sequential.c -lssl -lcrypto
RUN mpicc -o parteB parteB.c -lssl -lcrypto
RUN mpicc -o adaptive_search adaptive_search.c -lssl -lcrypto
RUN mpicc -o binary_search binary_search.c -lssl -lcrypto

# Crear un nuevo usuario
RUN adduser --disabled-password --gecos "" newparalela

# Cambiar al usuario newparalela
USER newparalela

# Establecer el directorio de trabajo para el usuario newparalela
WORKDIR /usr/src/app

# Ejecutar el programa paralelo por defecto (puedes cambiar esto según tus necesidades)
CMD ["mpirun", "-np", "4", "./bruteforce_mpi"]
