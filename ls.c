/* This program lists the files contained within the current working directory.
 *
 */

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

/*
 * sizeComparison
 */
int sizeComparison(const void *const dirstruct1, const void *const dirstruct2) {
    return strcoll((*(struct dirent **) dirstruct1)->d_name, (*(struct dirent**) dirstruct2)->d_name);
}
int dirsize(struct dirent *dp, DIR *dir) {
    int size = 0;
    while (dp != NULL) {
        ++size;
        dp = readdir(dir);
    }
    return size;
}

bool prefix(const char *pre, const char *str)
{
    if (strncmp(pre, str, strlen(pre)) == 0) {
        return true;
    } else {
        return false;
    }
}
/*
 * Filters out unnecessary files (such as . and ..) as well as files prefixed with ., since
 * this is not presented normally in ls.
 */
int filter(char *entry) {
    if ((strcoll(entry, ".") == 0)|| (strcoll(entry, "..") == 0) || prefix(".", entry)){
        return true;
    } else {
        return false;
    }
}
void printDir(struct dirent **dp, int size, bool aFlag){
    for (int i = 0; i < size; ++i) {
        if (filter(dp[i]->d_name) && !aFlag) continue; // not printing verbose
        fprintf(stderr, "%s\n", dp[i]->d_name);
    }
}
/*
 * Given a DIR object, dir, list the files within that directory.
 */
void list(DIR *dir, bool allFilter) {
    assert(dir != NULL);
    struct dirent *ent = readdir(dir);
    struct dirent **list;
    int size = dirsize(ent, dir);
    list = malloc((size * sizeof(*list)));
    if (list == NULL) { // somehow we ran out of memory, should never happen
        closedir(dir);
        perror("malloc");
        exit(1);
    }
    rewinddir(dir); // directory reset, necessary as the DIR object has already been exhausted
    size = 0; // resetting size for iteration
    while ((ent = readdir(dir)) != NULL) {
        list[size++] = ent;
    }
    qsort(list, size, sizeof(*list), sizeComparison);
    printDir(list, size, allFilter);
}


/* List the contents of the current working directory.
 *
 */
int main(int argc, char* argv[]) {
    bool aFlag = false;
    if (argc > 1 && strcoll(argv[1], "-a") == 0) {
        aFlag = true;
    } else if (argc > 1) {
        printf("An invalid flag was given.");
        exit(1);
    }
    DIR *d = opendir(".");
    if (d == NULL) {
        perror("diropen");
        exit(1);
    }

    list(d, aFlag);
    closedir(d);
    return 0;
}
