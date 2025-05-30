#include "ova-web.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cjson/cJSON.h>

#define FILE_PATH "preguntas.json"

char* leer_json_completo(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, file);
    fclose(file);
    return buffer;
}

char* obtener_pregunta_aleatoria() {
    char* buffer = leer_json_completo(FILE_PATH);
    if (!buffer) return strdup("[]");

    cJSON* array = cJSON_Parse(buffer);
    if (!cJSON_IsArray(array)) {
        free(buffer);
        return strdup("[]");
    }

    int total = cJSON_GetArraySize(array);
    if (total == 0) {
        cJSON_Delete(array);
        free(buffer);
        return strdup("[]");
    }

    srand(time(NULL));
    int index = rand() % total;
    cJSON* pregunta = cJSON_GetArrayItem(array, index);
    char* resultado = strdup(cJSON_Print(pregunta));

    cJSON_Delete(array);
    free(buffer);
    return resultado;
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

    free(buffer);
    cJSON_Delete(root);
    free(nuevo_json);
}

char* validar_respuesta_por_indice(const char* id, int indice) {
    char* buffer = leer_json_completo(FILE_PATH);
    if (!buffer) return strdup("{\"resultado\":false}");

    cJSON* array = cJSON_Parse(buffer);
    if (!cJSON_IsArray(array)) {
        free(buffer);
        return strdup("{\"resultado\":false}");
    }

    int total = cJSON_GetArraySize(array);
    for (int i = 0; i < total; i++) {
        cJSON* pregunta = cJSON_GetArrayItem(array, i);
        cJSON* pid = cJSON_GetObjectItem(pregunta, "id");
        if (pid && strcmp(pid->valuestring, id) == 0) {
            cJSON* opciones = cJSON_GetObjectItem(pregunta, "opciones");
            if (!cJSON_IsArray(opciones) || indice >= cJSON_GetArraySize(opciones)) {
                cJSON_Delete(array);
                free(buffer);
                return strdup("{\"resultado\":false,\"mensaje\":\"Índice inválido\"}");
            }

            cJSON* opcion = cJSON_GetArrayItem(opciones, indice);
            cJSON* esCorrecta = cJSON_GetObjectItem(opcion, "esCorrecta");

            if (cJSON_IsBool(esCorrecta) && cJSON_IsTrue(esCorrecta)) {
                cJSON_Delete(array);
                free(buffer);
                return strdup("{\"resultado\":true,\"mensaje\":\"¡Correcto!\"}");
            } else {
                cJSON_Delete(array);
                free(buffer);
                return strdup("{\"resultado\":false,\"mensaje\":\"Incorrecto\"}");
            }
        }
    }

    cJSON_Delete(array);
    free(buffer);
    return strdup("{\"resultado\":false,\"mensaje\":\"Pregunta no encontrada\"}");
}

void liberar_memoria(char* ptr) {
    free(ptr);
}

// --- Funciones JNI ---

// Helper para convertir jstring a char*
static char* jstringToCString(JNIEnv* env, jstring jStr) {
    if (!jStr) return NULL;
    const char* utf = (*env)->GetStringUTFChars(env, jStr, NULL);
    char* str = strdup(utf);
    (*env)->ReleaseStringUTFChars(env, jStr, utf);
    return str;
}

JNIEXPORT jstring JNICALL Java_ejemplojni_lib_OvaWebJNI_obtenerPreguntaAleatoria(JNIEnv *env, jobject obj) {
    char* resultado = obtener_pregunta_aleatoria();
    jstring jResultado = (*env)->NewStringUTF(env, resultado);
    liberar_memoria(resultado);
    return jResultado;
}

JNIEXPORT void JNICALL Java_ejemplojni_lib_OvaWebJNI_guardarPregunta(JNIEnv *env, jobject obj, jstring jJsonStr) {
    char* jsonStr = jstringToCString(env, jJsonStr);
    guardar_pregunta(jsonStr);
    free(jsonStr);
}

JNIEXPORT jstring JNICALL Java_ejemplojni_lib_OvaWebJNI_validarRespuestaPorIndice(JNIEnv *env, jobject obj, jstring jId, jint indice) {
    char* id = jstringToCString(env, jId);
    char* resultado = validar_respuesta_por_indice(id, indice);
    jstring jResultado = (*env)->NewStringUTF(env, resultado);
    liberar_memoria(resultado);
    free(id);
    return jResultado;
}

JNIEXPORT void JNICALL Java_ejemplojni_lib_OvaWebJNI_liberarMemoria(JNIEnv *env, jobject obj, jlong ptr) {
    free((void*)ptr);
}