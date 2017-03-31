#include <jni.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <android/log.h>
#include "native-util.h"
#include "Python.h"

#define ENTRYPOINT_MAXLEN 128
#define LOG(n, x) __android_log_write(ANDROID_LOG_INFO, (n), (x))
#define LOGP(x) LOG("python", (x))

extern "C"
JNIEXPORT jint JNICALL Java_com_example_bko_MainActivity_nativePythonStart (
        JNIEnv *env, jobject thiz, jstring j_android_private,
        jstring j_android_argument, jstring j_service_entrypoint,
        jstring j_python_name, jstring j_python_home, jstring j_python_path,
        jstring j_arg) {

    jboolean iscopy;
    const char *android_private = env->GetStringUTFChars(j_android_private, &iscopy);
    const char *android_argument = env->GetStringUTFChars(j_android_argument, &iscopy);
    const char *service_entrypoint = env->GetStringUTFChars(j_service_entrypoint, &iscopy);
    const char *python_name = env->GetStringUTFChars(j_python_name, &iscopy);
    const char *python_home = env->GetStringUTFChars(j_python_home, &iscopy);
    const char *python_path = env->GetStringUTFChars(j_python_path, &iscopy);
    const char *arg = env->GetStringUTFChars(j_arg, &iscopy);
    
    const char *env_argument = NULL;
    const char *env_entrypoint = NULL;
    const char *env_logname = NULL;
    char entrypoint[ENTRYPOINT_MAXLEN];
    int ret = 0;
    FILE *fd;

    LOGP("Initialize Python for Android");
    env_argument = android_argument;
    env_entrypoint = service_entrypoint;
    env_logname = python_name;

    if (env_logname == NULL) {
        env_logname = "python";
        setenv("PYTHON_NAME", "python", 1);
    }

    LOGP("Changing directory to the one provided by ANDROID_ARGUMENT");
    LOGP(env_argument);
    chdir(env_argument);

    Py_SetProgramName(L"android_python");

    PyImport_AppendInittab("androidembed", initandroidembed);

    LOGP("Preparing to initialize python");

    if (dir_exists("assets/python/")) {
        LOGP("crystax_python exists");
        char paths[256];
        snprintf(paths, 256,
                 "%s/assets/python/stdlib.zip:%s/assets/python/modules",
                 env_argument, env_argument);
        /* snprintf(paths, 256, "%s/stdlib.zip:%s/modules", env_argument,
         * env_argument); */
        LOGP("calculated paths to be...");
        LOGP(paths);

        wchar_t *wchar_paths = Py_DecodeLocale(paths, NULL);
        Py_SetPath(wchar_paths);

        LOGP("set wchar paths...");
    } else {
        LOGP("crystax_python does not exist");
    }

    Py_Initialize();

    LOGP("Initialized python");

    /* ensure threads will work.
     */
    LOGP("AND: Init threads");
    PyEval_InitThreads();

    PyRun_SimpleString("import androidembed\nandroidembed.log('testing python "
                               "print redirection')");

    /* inject our bootstrap code to redirect python stdin/stdout
     * replace sys.path with our path
     */
    PyRun_SimpleString("import sys, posix\n");


    if (dir_exists("assets/python/")) {
        char add_site_packages_dir[256];
        snprintf(add_site_packages_dir, 256,
                 "sys.path.append('%s/assets/python/site-packages')",
                 env_argument);

        PyRun_SimpleString("import sys\n"
                                   "sys.argv = ['notaninterpreterreally']\n"
                                   "from os.path import realpath, join, dirname");
        PyRun_SimpleString(add_site_packages_dir);
        /* "sys.path.append(join(dirname(realpath(__file__)), 'site-packages'))") */
        PyRun_SimpleString("sys.path = ['.'] + sys.path");
    }

    PyRun_SimpleString(PYTHON_SIMPLE_SCRIPT.c_str());

    LOGP("AND: Ran string");

    /* run it !
     */
    LOGP("Run user program, change dir and execute entrypoint");

    /* Get the entrypoint, search the .pyo then .py
     */
    char *dot = strrchr(env_entrypoint, '.');
    if (dot <= 0) {
        LOGP("Invalid entrypoint, abort.");
        return -1;
    }
    if (strlen(env_entrypoint) > ENTRYPOINT_MAXLEN - 2) {
        LOGP("Entrypoint path is too long, try increasing ENTRYPOINT_MAXLEN.");
        return -1;
    }
    if (!strcmp(dot, ".pyo")) {
        if (!file_exists(env_entrypoint)) {
            /* fallback on .py */
            strcpy(entrypoint, env_entrypoint);
            entrypoint[strlen(env_entrypoint) - 1] = '\0';
            LOGP(entrypoint);
            if (!file_exists(entrypoint)) {
                LOGP("Entrypoint not found (.pyo, fallback on .py), abort");
                return -1;
            }
        } else {
            strcpy(entrypoint, env_entrypoint);
        }
    } else if (!strcmp(dot, ".py")) {
        /* if .py is passed, check the pyo version first */
        strcpy(entrypoint, env_entrypoint);
        entrypoint[strlen(env_entrypoint) + 1] = '\0';
        entrypoint[strlen(env_entrypoint)] = 'o';
        if (!file_exists(entrypoint)) {
            /* fallback on pure python version */
            if (!file_exists(env_entrypoint)) {
                LOGP("Entrypoint not found (.py), abort.");
                return -1;
            }
            strcpy(entrypoint, env_entrypoint);
        }
    } else {
        LOGP("Entrypoint have an invalid extension (must be .py or .pyo), abort.");
        return -1;
    }
    // LOGP("Entrypoint is:");
    // LOGP(entrypoint);
    fd = fopen(entrypoint, "r");
    if (fd == NULL) {
        LOGP("Open the entrypoint failed");
        LOGP(entrypoint);
        return -1;
    }

    /* run python !
     */
    ret = PyRun_SimpleFile(fd, entrypoint);

    if (PyErr_Occurred() != NULL) {
        ret = 1;
        PyErr_Print(); /* This exits with the right code if SystemExit. */
        PyObject *f = PySys_GetObject("stdout");
        if (PyFile_WriteString(
                "\n", f)) /* python2 used Py_FlushLine, but this no longer exists */
            PyErr_Clear();
    }

    /* close everything
     */
    Py_Finalize();
    fclose(fd);

    LOGP("Python for android ended.");


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