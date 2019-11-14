//
// Created by Dan Aimone on 11/12/19.
//

#include "filter.h"

char* fileType(struct stat fstat) {
    static const mode_t modeMask[] = {S_IFBLK, S_IFCHR, S_IFDIR, S_IFLNK, S_IFIFO, S_IFREG, S_IFSOCK};
    static char* modeChar[] = {"b", "c", "d", "l", "p", "f", "s", "?"};
    size_t modeSize = sizeof(modeMask) / sizeof(int);
    mode_t fileMode = fstat.st_mode;
    int i = 0;
    while (i < modeSize && modeMask[i] != (fileMode & S_IFMT)) {
        ++i;
    }
    return modeChar[i];
}

bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0 ? true : false;
}

int filter(char *entry) {
    return (strcoll(entry, ".") == 0) || (strcoll(entry, "..") == 0) || prefix(".", entry);
}

bool isFlag(const char *str) {
    return prefix("-", str);
}


