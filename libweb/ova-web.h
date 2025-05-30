#ifndef OVA_WEB_H
#define OVA_WEB_H

#ifdef __cplusplus
extern "C" {
#endif

// Funciones normales C para manejo JSON
char* leer_json_completo(const char *filename);
char* obtener_pregunta_aleatoria();
void guardar_pregunta(const char* json_str);
char* validar_respuesta_por_indice(const char* id, int indice);
void liberar_memoria(char* ptr);

// Funciones JNI (para que se declaren visibles desde Java)
#include <jni.h>
JNIEXPORT jstring JNICALL Java_ejemplojni_lib_OvaWebJNI_obtenerPreguntaAleatoria(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_ejemplojni_lib_OvaWebJNI_guardarPregunta(JNIEnv *, jobject, jstring);
JNIEXPORT jstring JNICALL Java_ejemplojni_lib_OvaWebJNI_validarRespuestaPorIndice(JNIEnv *, jobject, jstring, jint);
JNIEXPORT void JNICALL Java_ejemplojni_lib_OvaWebJNI_liberarMemoria(JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif

#endif
