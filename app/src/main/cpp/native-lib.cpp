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
    const char *env_argument = env->GetStringUTFChars(j_android_argument, &iscopy);
    const char *env_entrypoint = env->GetStringUTFChars(j_service_entrypoint, &iscopy);
    const char *env_logname = env->GetStringUTFChars(j_python_name, &iscopy);

    int ret = 0;
    FILE *fd;

    LOGP("Initialize Python for Android");

    if (env_logname == NULL) {
        env_logname = "python";
        setenv("PYTHON_NAME", "python", 1);
    }

    LOGP("Changing directory to: ");
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

    LOGP("Init threads");
    PyEval_InitThreads();

    PyRun_SimpleString("import androidembed\nandroidembed.log('testing python "
                               "print redirection')");

    // inject our bootstrap code to redirect python stdin/stdout replace sys.path with our path
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
        PyRun_SimpleString("sys.path = ['.'] + sys.path");
    }

    LOGP("Trying to run a simple script in String:");
    PyRun_SimpleString(PYTHON_SIMPLE_SCRIPT.c_str());
    LOGP("AND: Ran string");


    LOGP("Run user program, change dir and execute entrypoint");
    char entrypoint[ENTRYPOINT_MAXLEN];
    snprintf(entrypoint, 256, "%s/assets/python/main.py", env_argument);

    LOGP("Entrypoint is:");
    LOGP(entrypoint);

    fd = fopen(entrypoint, "r");
    if (fd == NULL) {
        LOGP("Open the entrypoint failed");
        LOGP(entrypoint);
        return -1;
    }

    // Run python from main.py
    ret = PyRun_SimpleFile(fd, entrypoint);

    if (PyErr_Occurred() != NULL) {
        ret = 1;
        PyErr_Print(); /* This exits with the right code if SystemExit. */
        PyObject *f = PySys_GetObject("stdout");
        if (PyFile_WriteString(
                "\n", f))
            PyErr_Clear();
    }

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