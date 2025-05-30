FROM gcc:latest

WORKDIR /app

# Instalar dependencias para cJSON y compilar
RUN apt-get update && \
    apt-get install -y git cmake make && \
    git clone https://github.com/DaveGamble/cJSON.git && \
    cd cJSON && mkdir build && cd build && \
    cmake .. && make && make install && \
    cd /app && rm -rf cJSON && \
    apt-get remove -y git cmake make && apt-get autoremove -y && apt-get clean

# Copiar archivos fuente
COPY ova-web.c .
COPY ova-web.h .

# Compilar la librería .so
RUN gcc -fPIC -c ova-web.c -o ova-web.o && \
    gcc -shared -o libova.so ova-web.o -lcjson

# Definir el path de librería para ejecución posterior
ENV LD_LIBRARY_PATH=/app
