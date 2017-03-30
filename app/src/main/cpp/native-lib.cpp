#include <jni.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <android/log.h>
#include "native-util.h"


#define ENTRYPOINT_MAXLEN 128
#define LOG(n, x) __android_log_write(ANDROID_LOG_INFO, (n), (x))
#define LOGP(x) LOG("python", (x))

extern "C"
JNIEXPORT jint JNICALL Java_com_sample_embedpython_pybridge_PyBridge_nativeStart(
        JNIEnv *env, jobject thiz, jstring j_android_private,
        jstring j_android_argument, jstring j_service_entrypoint,
        jstring j_python_name, jstring j_python_home, jstring j_python_path,
        jstring j_arg) {

    if (dir_exists("assets/python/")) {
        LOGP("DIR EXISTS");
    }

    return 0;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_bko_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Python interpreter initialized successfully!";
    return env->NewStringUTF(hello.c_str());
}