#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>

#define FILE_PATH "preguntas.json"
#define BUFFER_SIZE 10000

// Función para agregar una nueva pregunta al archivo JSON
void guardar_pregunta(const char* json_str) {
    FILE* file = fopen(FILE_PATH, "a+");
    if (!file) {
        printf("Error al abrir el archivo\n");
        return;
    }

    // Leer contenido actual del archivo
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, file);
    fclose(file);

    // Crear objeto JSON
    cJSON* root = cJSON_Parse(buffer);
    if (!root) {
        root = cJSON_CreateArray();  // Si el archivo está vacío, crear un arreglo vacío
    }

    // Parsear la pregunta que se recibe como string
    cJSON* nueva_pregunta = cJSON_Parse(json_str);  
    cJSON_AddItemToArray(root, nueva_pregunta);    // Agregar la nueva pregunta al arreglo

    // Guardar el arreglo de vuelta en el archivo
    char* nuevo_json = cJSON_Print(root);  
    file = fopen(FILE_PATH, "w");
    fputs(nuevo_json, file);  
    fclose(file);

    printf("Pregunta guardada con éxito.\n");

    // Liberar memoria
    free(buffer);
    cJSON_Delete(root);
    free(nuevo_json);
}

// Función para obtener una pregunta aleatoria del archivo JSON
void obtener_pregunta_aleatoria() {
    FILE* file = fopen(FILE_PATH, "r");
    if (!file) {
        printf("[]\n");  
        return;
    }

    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);

    // Parsear el archivo JSON
    cJSON* array = cJSON_Parse(buffer);
    if (!cJSON_IsArray(array)) {
        printf("[]\n");  
        free(buffer);
        return;
    }

    int total = cJSON_GetArraySize(array);  
    if (total == 0) {
        printf("[]\n");  
        free(buffer);
        cJSON_Delete(array);
        return;
    }

    srand(time(NULL));  
    int index = rand() % total;  
    cJSON* pregunta = cJSON_GetArrayItem(array, index);  

    char* resultado = cJSON_Print(pregunta);  
    printf("%s\n", resultado);  

    free(buffer);
    cJSON_Delete(array);
    free(resultado);
}

// Función para validar la respuesta de una pregunta
void validar_respuesta(const char* id, const char* respuesta) {
    FILE* file = fopen(FILE_PATH, "r");
    if (!file) {
        printf("{\"resultado\":false}\n");  
        return;
    }

    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);

    cJSON* array = cJSON_Parse(buffer);
    if (!cJSON_IsArray(array)) {
        printf("{\"resultado\":false}\n");  
        free(buffer);
        return;
    }

    int total = cJSON_GetArraySize(array);  
    for (int i = 0; i < total; i++) {
        cJSON* pregunta = cJSON_GetArrayItem(array, i);  
        cJSON* pid = cJSON_GetObjectItem(pregunta, "id");  
        cJSON* correcta = cJSON_GetObjectItem(pregunta, "respuesta_correcta");  

        if (pid && correcta && strcmp(pid->valuestring, id) == 0) {  
            if (strcmp(correcta->valuestring, respuesta) == 0) {  
                printf("{\"resultado\":true}\n");
            } else {
                printf("{\"resultado\":false}\n");  
            }
            free(buffer);
            cJSON_Delete(array);
            return;
        }
    }

    printf("{\"resultado\":false}\n");  
    free(buffer);
    cJSON_Delete(array);
}

// Función para leer un archivo JSON
void leer_json(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error al abrir el archivo\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, file);
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        printf("Error al parsear el JSON\n");
        free(buffer);
        return;
    }

    char *json_string = cJSON_Print(json);
    printf("Contenido JSON: %s\n", json_string);

    free(buffer);
    free(json_string);
    cJSON_Delete(json);
}
