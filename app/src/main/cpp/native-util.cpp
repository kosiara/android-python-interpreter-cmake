//
// Created by Bartosz Kosarzycki on 3/30/17.
//

#include <stdio.h>
#include <sys/stat.h>

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