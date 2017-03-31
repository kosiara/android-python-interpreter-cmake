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
        JNIEnv *env, jobject thiz,
        jstring j_android_argument, jstring j_script_entrypoint, jstring j_python_name) {

    jboolean iscopy;
    const char *env_argument = env->GetStringUTFChars(j_android_argument, &iscopy);
    const char *env_script_main_entrypoint = env->GetStringUTFChars(j_script_entrypoint, &iscopy);
    const char *env_logname = env->GetStringUTFChars(j_python_name, &iscopy);

    if (env_logname == NULL)
        env_logname = "python";

    int ret = 0;
    FILE *fd;
    LOGP("Initializing Python for Android ...");

    LOGP((std::string("Changing directory to: ") + env_argument).c_str());
    chdir(env_argument);

    Py_SetProgramName(L"android_python");
    PyImport_AppendInittab("androidembed", initandroidembed);

    LOGP("Preparing to initialize python ...");

    if (dir_exists("assets/python/")) {
        LOGP("crystax_python exists");
        char paths[256];
        snprintf(paths, 256, "%s/assets/python/stdlib.zip:%s/assets/python/modules", env_argument, env_argument);

        LOGP((std::string("Calculated paths: ") + paths).c_str());
        wchar_t *wchar_paths = Py_DecodeLocale(paths, NULL);
        Py_SetPath(wchar_paths);

        LOGP("Set wchar paths.");
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

    LOGP("Set system paths...");
    if (dir_exists("assets/python/")) {
        char add_site_packages_dir[256];
        snprintf(add_site_packages_dir, 256, "sys.path.append('%s/assets/python/site-packages')", env_argument);

        PyRun_SimpleString("import sys\n"
                                   "sys.argv = ['notaninterpreterreally']\n"
                                   "from os.path import realpath, join, dirname");
        PyRun_SimpleString(add_site_packages_dir);
        PyRun_SimpleString("sys.path = ['.'] + sys.path");
    }

    LOGP("Trying to run a simple script in String:");
    PyRun_SimpleString(PYTHON_SIMPLE_SCRIPT.c_str());
    LOGP("Ran string");


    LOGP("Run user program, change dir and execute entrypoint");
    char entrypoint[ENTRYPOINT_MAXLEN];
    snprintf(entrypoint, 256, "%s/%s", env_argument, env_script_main_entrypoint);

    LOGP((std::string("Entrypoint is: ") + entrypoint).c_str());

    fd = fopen(entrypoint, "r");
    if (fd == NULL) {
        LOGP((std::string("Open the entrypoint failed: ") + entrypoint).c_str());
        return -1;
    }

    // Run python from main.py
    ret = PyRun_SimpleFile(fd, entrypoint);

    if (PyErr_Occurred() != NULL) {
        ret = 1;
        PyErr_Print(); /* This exits with the right code if SystemExit. */
        PyObject *f = PySys_GetObject("stdout");
        if (PyFile_WriteString("\n", f))
            PyErr_Clear();
    }

    fclose(fd);

    //LOGP("Run Unit tests....");
    //runUnitTestFile(env_argument, "test_int.py");
    //runUnitTestFile(env_argument, "test_int_literal.py");

    Py_Finalize();

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