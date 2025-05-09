FROM gcc:latest

WORKDIR /app

# Instala y compila cJSON, luego limpia todo lo innecesario
RUN apt-get update && \
    apt-get install -y git cmake make && \
    git clone https://github.com/DaveGamble/cJSON.git && \
    cd cJSON && mkdir build && cd build && \
    cmake .. && make && make install && \
    cd /app && rm -rf cJSON && \
    apt-get remove -y git cmake make && apt-get autoremove -y && apt-get clean

# Copia el código fuente
COPY ova-web.c .

# Compila como librería dinámica
RUN gcc -fPIC -c ova-web.c -o ova-web.o && \
    gcc -shared -o libova.so ova-web.o

# (Opcional) definir variable para encontrar la .so en tiempo de ejecución
ENV LD_LIBRARY_PATH=/app
