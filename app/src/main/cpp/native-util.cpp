//
// Created by Bartosz Kosarzycki on 3/30/17.
//

#include <stdio.h>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <android/log.h>
#include "Python.h"

#define LOG(n, x) __android_log_write(ANDROID_LOG_INFO, (n), (x))
#define LOGP(x) LOG("python", (x))

static PyObject *androidembed_log(PyObject *self, PyObject *args) {
    char *logstr = NULL;
    if (!PyArg_ParseTuple(args, "s", &logstr)) {
        return NULL;
    }
    LOG(getenv("PYTHON_NAME"), logstr);
    Py_RETURN_NONE;
}

int dir_exists(char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        if (S_ISDIR(st.st_mode))
            return 1;
    }
    return 0;
}

int file_exists(const char *filename) {
    FILE *file;
    if (file = fopen(filename, "r")) {
        fclose(file);
        return 1;
    }
    return 0;
}

void runUnitTestFile(const char *env_argument, const char *unit_test_file) {
    FILE *fd; int ret; char python_tests_main[256];
    snprintf(python_tests_main, 256, "%s/assets/python/modules/test/%s", env_argument, unit_test_file);

    LOGP((std::string("Entrypoint is: ") + python_tests_main).c_str());

    file_exists(python_tests_main);

    fd = fopen(python_tests_main, "r");
    ret = PyRun_SimpleFile(fd, python_tests_main);
    //LOGP((std::string("Ran UnitTest : ") + python_tests_main).c_str());
}

static PyMethodDef AndroidEmbedMethods[] = {
        {"log", androidembed_log, METH_VARARGS, "Log on android platform"},
        {NULL, NULL, 0, NULL}};

static struct PyModuleDef androidembed = {PyModuleDef_HEAD_INIT, "androidembed",
                                          "", -1, AndroidEmbedMethods};

PyMODINIT_FUNC initandroidembed(void) {
    return PyModule_Create(&androidembed);
}
