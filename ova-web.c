#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>

#define FILE_PATH "preguntas.json"
#define BUFFER_SIZE 10000

char* leer_json_completo(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error al abrir el archivo: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

void guardar_pregunta(const char* json_str) {
    char* buffer = leer_json_completo(FILE_PATH);

    cJSON* root = NULL;
    if (buffer && strlen(buffer) > 0) {
        root = cJSON_Parse(buffer);
    }

    if (!root) {
        root = cJSON_CreateArray();
    }

    cJSON* nueva_pregunta = cJSON_Parse(json_str);
    cJSON_AddItemToArray(root, nueva_pregunta);

    char* nuevo_json = cJSON_Print(root);

    FILE* file = fopen(FILE_PATH, "w");
    fputs(nuevo_json, file);
    fclose(file);

    printf("Pregunta guardada con éxito.\n");

    free(buffer);
    cJSON_Delete(root);
    free(nuevo_json);
}

void obtener_pregunta_aleatoria() {
    char* buffer = leer_json_completo(FILE_PATH);
    if (!buffer) {
        printf("[]\n");
        return;
    }

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
    printf("Pregunta aleatoria:\n%s\n", resultado);

    free(buffer);
    cJSON_Delete(array);
    free(resultado);
}

void validar_respuesta_por_indice(const char* id, int indice) {
    char* buffer = leer_json_completo(FILE_PATH);
    if (!buffer) {
        printf("[]\n");
        return;
    }

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
        if (pid && strcmp(pid->valuestring, id) == 0) {
            cJSON* opciones = cJSON_GetObjectItem(pregunta, "opciones");
            if (!cJSON_IsArray(opciones) || indice >= cJSON_GetArraySize(opciones)) {
                printf("{\"resultado\":false, \"mensaje\":\"Índice de opción inválido\"}\n");
                cJSON_Delete(array);
                free(buffer);
                return;
            }

            cJSON* opcion = cJSON_GetArrayItem(opciones, indice);
            cJSON* esCorrecta = cJSON_GetObjectItem(opcion, "esCorrecta");

            if (cJSON_IsBool(esCorrecta) && cJSON_IsTrue(esCorrecta)) {
                printf("{\"resultado\":true, \"mensaje\":\"¡Respuesta correcta!\"}\n");
            } else {
                printf("{\"resultado\":false, \"mensaje\":\"Respuesta incorrecta.\"}\n");
            }

            cJSON_Delete(array);
            free(buffer);
            return;
        }
    }

    printf("{\"resultado\":false, \"mensaje\":\"Pregunta no encontrada\"}\n");
    cJSON_Delete(array);
    free(buffer);
}
