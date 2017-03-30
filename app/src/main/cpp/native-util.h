//
// Created by Bartosz Kosarzycki on 3/30/17.
//

#ifndef PYTHONINTERPRETEREXAMPLE_NATIVE_UTIL_H
#define PYTHONINTERPRETEREXAMPLE_NATIVE_UTIL_H

int dir_exists(char *filename);

int file_exists(const char *filename);

const std::string PYTHON_SIMPLE_SCRIPT = "class LogFile(object):\n"
        "    def __init__(self):\n"
        "        self.buffer = ''\n"
        "    def write(self, s):\n"
        "        s = self.buffer + s\n"
        "        lines = s.split(\"\\n\")\n"
        "        for l in lines[:-1]:\n"
        "            androidembed.log(l)\n"
        "        self.buffer = lines[-1]\n"
        "    def flush(self):\n"
        "        return\n"
        "sys.stdout = sys.stderr = LogFile()\n"
        "print('Android path', sys.path)\n"
        "import os\n"
        "print('os.environ is', os.environ)\n"
        "print('Android kivy bootstrap done. __name__ is', __name__)";

#endif //PYTHONINTERPRETEREXAMPLE_NATIVE_UTIL_H
